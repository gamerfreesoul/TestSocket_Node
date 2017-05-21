#include "../CNodeClient/CNodeClient.h"

CNodeClient nodeClient;

int main(int nArgs, char** args)
{
	if (!nodeClient.InitNodeClient())
		return false;

	bool bRunning = true;
	while (bRunning)
		bRunning = nodeClient.Process();

	nodeClient.Exit();
	return 0;
}