#include<winsock2.h>
#include<windows.h>
#include<io.h>
#include<bits/stdc++.h>
//Ҫʹ�������̣�Ҫ����ϵͳ�������ṩ��ͷ�ļ�
#pragma comment(lib,"ws2_32.lib")
using namespace std;
void sendHtml(SOCKET clifd, const char* filePath);
//���������룬�����������
void error_die(const char* str) {
	printf("[hint]%s failed:%d", str,WSAGetLastError());//��ȡ������
	exit(-1);
}
//��ʼ��
void initSocket() {
	// ����1.ָ��socket�汾 ����2.��һ����������
	WORD version = MAKEWORD(2, 2);
	WSADATA wsadata;
	if (0 != WSAStartup(version, &wsadata)) {
		error_die("WSAStartup");
	}
}
SOCKET listenClient(){
	//2.����socket
	//����1.ָ��IPЭ�� ipv4(AF_INET) ipv6(AF_INET6)
	//����2.���ݴ����ʽ�������������֣���ʽ��������ݱ����� 
	//����3.����Э�飬                   TCP    ��    UDP
	SOCKET serfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serfd == INVALID_SOCKET) {
		error_die("socket");
	}
	//3.��IP��ַ�Ͷ˿ں�
	//����1.ָ����socket
	//����2.IP��ַ�Ͷ˿ں�
	//����3.
	struct sockaddr_in serAddr;
	/*
	typedef struct sockaddr_in {
	USHORT sin_port;
	IN_ADDR sin_addr;
	CHAR sin_zero[8];
	} SOCKADDR_IN, *PSOCKADDR_IN;
	*/
	serAddr.sin_family = AF_INET;//����ʹ���socket��ʱ��һ��
	//��˴洢--һ�������С�˴洢--һ��������htons�ѱ����ֽ���תΪ�����ֽ���
	serAddr.sin_port = htons(80);       //[0-65535) 0-1024��ϵͳ�����˿ںţ�һ�㲻�ã�80��http������ר��
	serAddr.sin_addr.S_un.S_addr = INADDR_ANY;//�󶨱������������
	if (SOCKET_ERROR == bind(serfd, (struct sockaddr*)&serAddr, sizeof(serAddr))) {
		error_die("bind");
	}
	//4.����
	listen(serfd, 10);
	return serfd;
}
void jieshou(SOCKET clifd){
	//��clifd��������
	char recvBuf[1024] = "";
	if (recv(clifd, recvBuf, sizeof(recvBuf), 0)<=0) {
		error_die("recv offline");
	}
	SYSTEMTIME st;//���屾��ʱ��������ñ���Ϊ�ṹ��
	GetLocalTime(&st);//��ȡ����ʱ�亯��������Ϊʱ�����ָ��
	cout<<"time:"<<st.wYear<<"��"<<st.wMonth<<"��"<<st.wDay<<"��"<<st.wHour<<"ʱ"<<st.wMinute<<"��"<<st.wSecond<<"��"<<endl;
	std::cout<<"recvBuf:";
	cout<<recvBuf<<std::endl;
	fstream r1;//������־
	r1.open("��־.txt",ios::app);
	r1<<"have a new connect...\ntime:"<<st.wYear<<"��"<<st.wMonth<<"��"<<st.wDay<<"��"<<st.wHour<<"ʱ"<<st.wMinute<<"��"<<st.wSecond<<"��"<<'\n'<<recvBuf<<endl;
	fstream cache;
	cache.open("cache.txt",ios::out);
	cache<<recvBuf;
	cache.close();
	cache.clear();
	cache.open("cache.txt");
	string a;
	getline(cache,a);
	//ֱ�Ӹ��ͻ��˷����ı�
	char filePath[128] = "";
	string b="";
	for(int i=4;i<a.size()-9;i++){
		b+=a[i];
	}
	cache.close();
	cache.clear();
	//ѡ��һ����ҳ����
	strcpy(filePath,&("htmlFiles"+b)[0]);
	//������ҳ
	sendHtml(clifd, filePath);
	closesocket(clifd);//������ֱ�ӹرգ�http�������ӵ�
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
	//1.��ʼ�������wsa windows socket ansyc->windows�첽�׽���
	initSocket();
	SOCKET serfd = listenClient();
	printf("hjsxhst2022 & sljpg2022 ��http��������\n");
	//5.��������
	struct sockaddr_in cliAddr;
	int len = sizeof(cliAddr);
	while (1) {
		SOCKET clifd = accept(serfd, (struct sockaddr*)&cliAddr, &len);//������
		//SOCKET clifd = accept(serfd, NULL, NULL);
		if (clifd == INVALID_SOCKET) {
			error_die("accept");
		}
		printf("have a new connect...\n");
		//6.������������
		//ֱ�Ӹ��ͻ��˷����ı�
		//char sendData[1024] = "<html><body><h1>zhinenؼ</h1></body></html>";
		//send(clifd, sendData, strlen(sendData), 0);
		accept_request(clifd);
	}
	//7.�ر����ӣ����������
	closesocket(serfd);
	WSACleanup();
	return 0;
}
