#include "NodeMgr\CNodeMgr.h"

CNodeMgr nodeMgr;

int main(int nArg, char** args)
{
	if (!nodeMgr.InitNodeMgr())
		return -1;
	
	bool bRunning = true;
	while (bRunning)
		bRunning = nodeMgr.Process();

	nodeMgr.Exit();
	return 0;
}

