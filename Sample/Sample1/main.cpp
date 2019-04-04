
#include "../Logger/PerformanceLogger.h"

int main()
{
	PerformanceLogger::getInstance()->startLogger("logTest.csv");

	ProcessingTimeLogger logMain("main");

	Sleep(300);

	{
		ProcessingTimeLogger log("Test1");
		Sleep(200);
	}

	Sleep(150);

	{
		ProcessingTimeLogger log("Test2");
		Sleep(500);
	}

	Sleep(600);

	return 0;
}