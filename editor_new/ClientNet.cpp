#include "ClientNet.h"  



int CClientNet::Connect(int port, const char* address)
{
	int rlt = 0;

	//用于记录错误信息并输出  
	int iErrMsg;
	//启动WinSock  
	WSAData wsaData;
	iErrMsg = WSAStartup(MAKEWORD(1, 1), &wsaData);
	if (iErrMsg != NO_ERROR)
		//有错误  
	{
		printf("failed with wsaStartup error : %d\n", iErrMsg);

		rlt = 1;
		return rlt;
	}

	//创建Socket  
	m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_sock == INVALID_SOCKET)
		//创建Socket失败  
	{ 
		printf("socket failed with error : %d\n", WSAGetLastError());

		rlt = 2;
		return rlt;
	}

	//目标服务器数据  
	sockaddr_in sockaddrServer;
	sockaddrServer.sin_family = AF_INET;
	sockaddrServer.sin_port = port;
	sockaddrServer.sin_addr.s_addr = inet_addr(address);

	//连接,sock与目标服务器连接  
	iErrMsg = connect(m_sock, (sockaddr*)&sockaddrServer, sizeof(sockaddrServer));
	if (iErrMsg < 0)
	{
		printf("connect failed with error : %d\n", iErrMsg);

		rlt = 3;
		return rlt;
	}

	return rlt;
}




int CClientNet::SendMsg(const char* msg, int len)
{
	int rlt = 0;

	int iErrMsg = 0;

	//发送消息，指定sock发送消息  
	iErrMsg = send(m_sock, msg, len, 0);
	if (iErrMsg < 0)
		//发送失败  
	{
		printf("send msg failed with error : %d\n", iErrMsg);
		rlt = 1;
		return rlt;
	}

	return rlt;
}

int CClientNet::request(){
	char request_msg[20] = "request";
	SendMsg(request_msg, sizeof((request_msg)));
	char buf[20];
	int rval = recv(m_sock, buf, 20, 0);
	if (strncmp("empty", buf, 20) == 0){
		return 1;
	}
	else return 0;
}

int CClientNet::SendFile(const char*filename)
{
	int rlt = 0;

	int iErrMsg = 0;
	FILE *f = new FILE;
	fopen_s(&f,filename,"r");
	char line[4096];
	size_t len = 4096;

	if (f == NULL){
		printf("file don't exist\n");
		return 1;
	}
	int num = 0;
	memset(line, 0, sizeof(char)*4096);
	while (!feof(f))//fgets(line,len,f) != NULL)
	{
		num = fread(line, 1, 4096, f);
		iErrMsg = send(m_sock, line, num, 0);
		
		if (iErrMsg < 0)
			//发送失败  
		{
			printf("send msg failed with error : %d\n", iErrMsg);

			rlt = 1;
			return rlt;
		}

		memset(line, 0, sizeof(char)*4096);
	}
	fclose(f);
	return rlt;
}

void CClientNet::Close()
{
	closesocket(m_sock);
}