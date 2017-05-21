#ifndef _NODE_MGR_H_
#define _NODE_MGR_H_

#include <list>
#include "../../Shared/socketDef.h"
#include "../../Shared/CTypeDef.h"

#define TELNET_PORT 9502
#define CLIENT_PORT 9503

class CSocketClient
{
public:
	CSocketClient();
	~CSocketClient();

	void Exit();

	bool KeepAlive();
	bool OnClose();

	int nConnSocket;
	SOCKADDR_IN clientAddr;
	DWORD checkAliveTime;
};

class CNodeMgr
{
public:
	CNodeMgr();
	~CNodeMgr();

	bool InitNodeMgr();
	void Exit();

	bool SocketStartUp(int& nScoket, int port);
	bool Process();
	void CheckTelnetAccept();
	void CheckClientAccept();

	int OnRecv(int socket, char* buff, int len);
	int OnSend(int socket, const char* buff, int len);

	//telnet ¼àÌýsocket
	int nSvrTelSocket;
	//¿Í»§¶Ë¼àÌýsocket
	int nSvrClientSocket;

private:
	std::list<CSocketClient> telSocketList;
	std::list<CSocketClient> clientSocketList;
};


#endif //_NODE_MGR_H_
