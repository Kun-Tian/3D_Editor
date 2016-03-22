#ifndef CLIENTNET_H
#define CLIENTNET_H

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>  
#include<windows.h>  
#include <stdlib.h>

#pragma comment(lib, "Ws2_32.lib")  


class CClientNet
{
public:
	//连接上指定服务器  
	int Connect(int port, const char* address);
	//发送信息  
	int SendMsg(const char* msg, int len);
	int SendFile(const char*filename);
	int request();
	//关闭  
	void Close();

private:
	SOCKET m_sock;
};
#endif // CLIENTNET_H