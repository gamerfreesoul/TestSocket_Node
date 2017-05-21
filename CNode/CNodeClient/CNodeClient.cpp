#include "CNodeClient.h"
#include "stdio.h"
#include "../../Shared/socketDef.h"

#define SERVER_PORT 9503

CNodeClient::CNodeClient()
{
	nClientSocket = 0;
	byState = eNone;
}

CNodeClient::~CNodeClient()
{

}

bool CNodeClient::InitNodeClient()
{
	int ret = 0;
#ifdef WIN32
	WORD vVersion;
	WSADATA wsaData;
	vVersion = MAKEWORD(2, 2);
	ret = WSAStartup(vVersion, &wsaData);
	if (ret != 0)
		return false;

	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wHighVersion) != 2)
	{
		WSACleanup();
		return false;
	}
#endif

	nClientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (nClientSocket == INVALID_SOCKET)
	{
		printf("Client Socket Create Err.\n");
		return false;
	}

	//设置非阻塞模式
	u_long arg = 1;
	ret = IOCTLSOCKET(nClientSocket, FIONBIO, &arg);
	if (ret == SOCKET_ERROR)
	{
		printf("Set NoBlock Socket Err!\n");
		return false;
	}

	byState = eReadConnect;
	return true;
}

bool CNodeClient::Connect()
{
	bool bResult = true;
	int ret = 0;
	SOCKADDR_IN tempAddr;
	tempAddr.sin_family = AF_INET;
	tempAddr.sin_port = htons(SERVER_PORT);
	tempAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

	ret = connect(nClientSocket, (sockaddr*)&tempAddr, sizeof(tempAddr));
	if (ret == SOCKET_ERROR)
	{
		int err = GETERROR;
		if (err == WSAEWOULDBLOCK)
			bResult = true;
#ifdef WIN32
		else if (err == WSAEISCONN)
		{
			byState = eConnected;
			printf("OnConnect socket %d ip %s port %d\n", nClientSocket, inet_ntoa(tempAddr.sin_addr), SERVER_PORT);
		}
		else
		{
			printf("connect Err. ip %s port %d ErrCode %d\n", inet_ntoa(tempAddr.sin_addr), SERVER_PORT, GETERROR);
			bResult = false;
		}
#else
		printf("connect Err. ip %s port %d ErrCode %d\n", inet_ntoa(tempAddr.sin_addr), SERVER_PORT, GETERROR);
		bResult = false;
#endif
		
	}
	else
	{
		byState = eConnected;
		printf("OnConnect socket %d ip %s port %d\n", nClientSocket, inet_ntoa(tempAddr.sin_addr), SERVER_PORT);
	}
	
	return bResult;
}

void CNodeClient::Exit()
{
	CLOSESOCKET(nClientSocket);
#ifdef WIN32
	WSACleanup();
#endif
}

int CNodeClient::OnRecv(int socket, char* buff, int len)
{
	int ret = recv(socket, buff, len, 0);

	if (ret == SOCKET_ERROR)
	{
		int err = GETERROR;
		if (err == WSAEWOULDBLOCK) return 0;
		return -1;
	}

	return ret;
}

int CNodeClient::OnSend(int socket, const char* buff, int len)
{
	int ret = send(socket, buff, len, 0);

	if (ret == SOCKET_ERROR)
	{
		int err = GETERROR;
		if (err == WSAEWOULDBLOCK) return 0;
		return -1;
	}

	return ret;
}

bool CNodeClient::Process()
{
	bool bResult = true;

	if (byState == eReadConnect)
		byState = eConnecting;
	else if (byState == eConnecting)
	{
		bResult = Connect();
	}
	else if (byState == eConnected)
	{
		char buff[1024] = { 0 };
		int ret = OnRecv(nClientSocket, buff, 1024);
		if (ret > 0)
		{
			printf("%s\n", buff);
		}
		else if (ret == SOCKET_ERROR)
		{
			CLOSESOCKET(nClientSocket);
			nClientSocket = 0;
			bResult = false;
		}
	}
	else
		bResult = false;

	return bResult;
}