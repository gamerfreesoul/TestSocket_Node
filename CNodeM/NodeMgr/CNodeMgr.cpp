#include "CNodeMgr.h"
#include "../../Shared/CMemPacket/CMemPakcet.h"

CSocketClient::CSocketClient()
{
	nConnSocket = -1;
	memset(&clientAddr, 0, sizeof(clientAddr));
}
CSocketClient::~CSocketClient()
{
}

void CSocketClient::Exit()
{
	CLOSESOCKET(nConnSocket);
}

bool CSocketClient::KeepAlive()
{
	return true;
}
bool CSocketClient ::OnClose()
{
	printf("close ip %s\n", inet_ntoa(clientAddr.sin_addr));
	memset(&clientAddr, 0, sizeof(clientAddr));
	CLOSESOCKET(nConnSocket);
	return true;
}


CNodeMgr::CNodeMgr()
{
	nSvrTelSocket = 0;
	nSvrClientSocket = 0;

	telSocketList.clear();
	clientSocketList.clear();
}

CNodeMgr::~CNodeMgr()
{
	CLOSESOCKET(nSvrTelSocket);
	CLOSESOCKET(nSvrClientSocket);

	for (auto it = telSocketList.begin(); it != telSocketList.end(); ++it)
	{
		CSocketClient& sClient = *it;
		sClient.OnClose();
	}
	telSocketList.clear();

	for (auto it = clientSocketList.begin(); it != clientSocketList.end(); ++it)
	{
		CSocketClient& sClient = *it;
		sClient.OnClose();
	}
	clientSocketList.clear();

#ifdef WIN32
	WSACleanup();
#endif
}

bool CNodeMgr::InitNodeMgr()
{
#ifdef WIN32
	int ret = 0;

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

	if (!SocketStartUp(nSvrTelSocket, TELNET_PORT))
	{
		printf("Telnet Socket Start up Err.\n");
		return false;
	}

	if (!SocketStartUp(nSvrClientSocket, CLIENT_PORT))
	{
		printf("Telnet Socket Start up Err.\n");
		return false;
	}

	return true;
}

void CNodeMgr::Exit()
{
	CLOSESOCKET(nSvrTelSocket);
	CLOSESOCKET(nSvrClientSocket);

	for (auto it = telSocketList.begin(); it != telSocketList.end(); ++it)
	{
		CSocketClient& sClient = *it;
		sClient.OnClose();
	}
	telSocketList.clear();

	for (auto it = clientSocketList.begin(); it != clientSocketList.end(); ++it)
	{
		CSocketClient& sClient = *it;
		sClient.OnClose();
	}
	clientSocketList.clear();

#ifdef WIN32
	WSACleanup();
#endif
}

bool CNodeMgr::SocketStartUp(int& nScoket, int port)
{
	int ret = 0;
	//创建telnet socket
	nScoket = socket(AF_INET, SOCK_STREAM, 0);
	if (nScoket == INVALID_SOCKET)
	{
		printf("socket create err.\n");
		return false;
	}
	//设置非阻塞模式
	u_long arg = 1;
	ret = IOCTLSOCKET(nScoket, FIONBIO, &arg);
	if (ret == SOCKET_ERROR)
	{
		printf("Set NoBlock Socket Err!\n");
		return false;
	}
	//绑定
	SOCKADDR_IN telnetAddr;
	telnetAddr.sin_family = AF_INET;
	telnetAddr.sin_port = htons(port);
	telnetAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	ret = bind(nScoket, (SOCKADDR *)&telnetAddr, sizeof(telnetAddr));
	if (ret == SOCKET_ERROR)
	{
		printf("Bind Telnet Socket Err!\n");
		return false;
	}

	//监听telnet 端口
	ret = listen(nScoket, 5);
	if (ret == SOCKET_ERROR)
	{
		printf("Listen Telnet Socket Err!\n");
		return false;
	}
	else
		printf("Listen port %d begin.\n", port);

	return true;
}

void CNodeMgr::CheckTelnetAccept()
{
	SOCKADDR_IN tempAddr;
	socklen_t nLen = sizeof(tempAddr);
	int nTempSocket = accept(nSvrTelSocket, (struct sockaddr*)&tempAddr, &nLen);
	if (nTempSocket != INVALID_SOCKET)
	{
		CSocketClient sClient;
		sClient.nConnSocket = nTempSocket;
		sClient.clientAddr = tempAddr;

		printf("Telnet connect socket %d ip %s \n", sClient.nConnSocket, inet_ntoa(sClient.clientAddr.sin_addr));
		telSocketList.push_back(sClient);
	}
}

void CNodeMgr::CheckClientAccept()
{
	SOCKADDR_IN tempAddr;
	socklen_t nLen = sizeof(tempAddr);
	int nTempSocket = accept(nSvrClientSocket, (struct sockaddr*)&tempAddr, &nLen);
	if (nTempSocket != INVALID_SOCKET)
	{
		CSocketClient sClient;
		sClient.nConnSocket = nTempSocket;
		sClient.clientAddr = tempAddr;

		printf("Client connect socket %d ip %s\n", sClient.nConnSocket, inet_ntoa(sClient.clientAddr.sin_addr));
		clientSocketList.push_back(sClient);
	}
}

bool CNodeMgr::Process()
{
	int ret = 0;
	bool bResult = true;
	CheckTelnetAccept();
	CheckClientAccept();

	std::list<std::list<CSocketClient>::iterator> teldel;
	std::list<std::list<CSocketClient>::iterator> clientdel;

	for (auto it = telSocketList.begin(); it != telSocketList.end(); ++it)
	{
		char buff[1024] = { 0 };
		CSocketClient sClient = *it;
		ret = OnRecv(sClient.nConnSocket, buff, 1024);
		if (ret > 0)
		{
			if (strcmp(buff, "\r") != 0)
			{
				printf("ip %s buff %s\n", inet_ntoa(sClient.clientAddr.sin_addr), buff);
				CMemPacket pPkt;
				pPkt.BeginWrite();
				if (strcmp(buff, "ls") == 0)
				{
					pPkt.Write(EVENT_S2C_LS);
					pPkt.Write("aaaaaaa");
					pPkt.Write("bbbbbbb");
					pPkt.Write("ccccccc");
				}
				for (auto clientit = clientSocketList.begin(); clientit != clientSocketList.end(); ++clientit)
				{
					CSocketClient clientSock = *clientit;
					ret = OnSend(clientSock.nConnSocket, pPkt.GetData(), pPkt.GetCurByte());
					if (ret == SOCKET_ERROR)
					{
						clientSock.OnClose();
						clientdel.push_back(clientit);
					}
				}

				if (strcmp(buff, "exit") == 0)
				{
					sClient.OnClose();
					teldel.push_back(it);
				}
			}
		}
		else if (ret == 0)
		{
			if (GETERROR == 0)
			{
				sClient.OnClose();
				teldel.push_back(it);
			}
		}
		else if (ret < 0)
		{
			sClient.OnClose();
			teldel.push_back(it);
		}
	}

	for (auto it = teldel.begin(); it != teldel.end(); ++it)
		telSocketList.erase(*it);

	for (auto it = clientdel.begin(); it != clientdel.end(); ++it)
		clientSocketList.erase(*it);

	return bResult;
}

int CNodeMgr::OnRecv(int socket, char* buff, int len)
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

int CNodeMgr::OnSend(int socket, const char* buff, int len)
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

