#ifndef _NODE_CLIENT_H_
#define _NODE_CLIENT_H_

#include "../../Shared/CTypeDef.h"

class CNodeClient
{
	enum ENodeClientState
	{
		eNone,
		eReadConnect,
		eConnecting,
		eConnected,
	};

public:
	CNodeClient();
	~CNodeClient();

	bool InitNodeClient();
	void Exit();
	
	bool Connect();
	int OnRecv(int socket, char* buff, int len);
	int OnSend(int socket, const char* buff, int len);
	bool Process();

private:
	int nClientSocket;
	BYTE byState;
};

#endif //_NODE_CLIENT_H_
