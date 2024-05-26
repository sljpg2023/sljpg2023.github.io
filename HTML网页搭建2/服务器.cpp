#include<winsock2.h>
#include<windows.h>
#include<io.h>
#include<bits/stdc++.h>
//要使用网络编程，要包含系统给我们提供的头文件
#pragma comment(lib,"ws2_32.lib")
using namespace std;
void sendHtml(SOCKET clifd, const char* filePath);
//输出错误代码，并返回输出码
void error_die(const char* str) {
	printf("[hint]%s failed:%d", str,WSAGetLastError());//获取错误码
	exit(-1);
}
//初始化
void initSocket() {
	// 参数1.指定socket版本 参数2.是一个传出参数
	WORD version = MAKEWORD(2, 2);
	WSADATA wsadata;
	if (0 != WSAStartup(version, &wsadata)) {
		error_die("WSAStartup");
	}
}
SOCKET listenClient(){
	//2.创建socket
	//参数1.指定IP协议 ipv4(AF_INET) ipv6(AF_INET6)
	//参数2.数据传输格式，常见的有两种，流式传输和数据报传输 
	//参数3.传输协议，                   TCP    和    UDP
	SOCKET serfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serfd == INVALID_SOCKET) {
		error_die("socket");
	}
	//3.绑定IP地址和端口号
	//参数1.指定的socket
	//参数2.IP地址和端口号
	//参数3.
	struct sockaddr_in serAddr;
	/*
	typedef struct sockaddr_in {
	USHORT sin_port;
	IN_ADDR sin_addr;
	CHAR sin_zero[8];
	} SOCKADDR_IN, *PSOCKADDR_IN;
	*/
	serAddr.sin_family = AF_INET;//必须和创建socket的时候一样
	//大端存储--一般网络和小端存储--一般计算机，htons把本地字节序转为网络字节序
	serAddr.sin_port = htons(80);       //[0-65535) 0-1024是系统保留端口号，一般不用，80是http服务器专用
	serAddr.sin_addr.S_un.S_addr = INADDR_ANY;//绑定本机任意服务器
	if (SOCKET_ERROR == bind(serfd, (struct sockaddr*)&serAddr, sizeof(serAddr))) {
		error_die("bind");
	}
	//4.监听
	listen(serfd, 10);
	return serfd;
}
void jieshou(SOCKET clifd){
	//从clifd接受数据
	char recvBuf[1024] = "";
	if (recv(clifd, recvBuf, sizeof(recvBuf), 0)<=0) {
		error_die("recv offline");
	}
	SYSTEMTIME st;//定义本地时间变量，该变量为结构体
	GetLocalTime(&st);//获取本地时间函数，参数为时间变量指针
	cout<<"time:"<<st.wYear<<"年"<<st.wMonth<<"月"<<st.wDay<<"日"<<st.wHour<<"时"<<st.wMinute<<"分"<<st.wSecond<<"秒"<<endl;
	std::cout<<"recvBuf:";
	cout<<recvBuf<<std::endl;
	fstream r1;//保存日志
	r1.open("日志.txt",ios::app);
	r1<<"have a new connect...\ntime:"<<st.wYear<<"年"<<st.wMonth<<"月"<<st.wDay<<"日"<<st.wHour<<"时"<<st.wMinute<<"分"<<st.wSecond<<"秒"<<'\n'<<recvBuf<<endl;
	fstream cache;
	cache.open("cache.txt",ios::out);
	cache<<recvBuf;
	cache.close();
	cache.clear();
	cache.open("cache.txt");
	string a;
	getline(cache,a);
	//直接给客户端发送文本
	char filePath[128] = "";
	string b="";
	for(int i=4;i<a.size()-9;i++){
		b+=a[i];
	}
	cache.close();
	cache.clear();
	//选择一个网页发送
	strcpy(filePath,&("htmlFiles"+b)[0]);
	//发送网页
	sendHtml(clifd, filePath);
	closesocket(clifd);//发送完直接关闭，http是无连接的
}
void accept_request(SOCKET clifd) {
	thread jieshous(jieshou,clifd);
	jieshous.detach();
}
void sendHtml(SOCKET clifd, const char * filePath) {
	FILE* pr = fopen(filePath, "r");
	if (pr==NULL) {
		//error_die("fopen");
		sendHtml(clifd, "htmlFiles\\error\\404.html");
		fclose(pr);
		return;
	}
	char data[1024] = "";
	do {
		fgets(data, 1024, pr);
		send(clifd, data, strlen(data), 0);
	} while (!feof(pr));
	fclose(pr);
}
int main()
{
	//1.初始化网络库wsa windows socket ansyc->windows异步套接字
	initSocket();
	SOCKET serfd = listenClient();
	printf("hjsxhst2022 & sljpg2022 的http服务器！\n");
	//5.接收链接
	struct sockaddr_in cliAddr;
	int len = sizeof(cliAddr);
	while (1) {
		SOCKET clifd = accept(serfd, (struct sockaddr*)&cliAddr, &len);//阻塞的
		//SOCKET clifd = accept(serfd, NULL, NULL);
		if (clifd == INVALID_SOCKET) {
			error_die("accept");
		}
		printf("have a new connect...\n");
		//6.处理链接请求
		//直接给客户端发送文本
		//char sendData[1024] = "<html><body><h1>zhinen丶</h1></body></html>";
		//send(clifd, sendData, strlen(sendData), 0);
		accept_request(clifd);
	}
	//7.关闭链接，清理网络库
	closesocket(serfd);
	WSACleanup();
	return 0;
}
