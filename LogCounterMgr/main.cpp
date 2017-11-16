#include "LogCounterMgr.h"

int main(int argc, char* argv[])
{
	//if (argc != 2)
	//{
	//	fprintf(stderr, "Error args! Input dir\n");
	//	return 0;
	//}

	LogCounterMgr logCounterMgr;
	if (!logCounterMgr.OpenFile("C:/Users/Administrator/Desktop/LogCounterMgr/test/WorldServer_20170922.log"))
	{
		return 0;
	}

	logCounterMgr.GenCounterDict();

	return 1;
}
