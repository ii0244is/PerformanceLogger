
#include "../Logger/PerformanceLogger.h"

std::mutex mutexSection1;
std::mutex mutexSection2;

void criticalSection1()
{
	ProcessingTimeLogger log("CriticalSection1");

	// 無意味な計算をして時間を稼ぐ。
	// コンパイル時、最適化オプションは OFF にしておく。
	for (int i = 0; i < 300000; ++i)
	{
		double a, b, c;
		a = (double)(rand() * i);
		b = (double)(rand() * i);
		c = a * a + b * b;
	}
}

void criticalSection2()
{
	ProcessingTimeLogger log("CriticalSection2");

	// 無意味な計算をして時間を稼ぐ。
	// コンパイル時、最適化オプションは OFF にしておく。
	for (int i = 0; i < 450000; ++i)
	{
		double a, b, c;
		a = (double)(rand() * i);
		b = (double)(rand() * i);
		c = a * a + b * b;
	}
}

void workerThread1()
{
	ProcessingTimeLogger workerlog("WorkerThread1");

	// クリティカルセクション１に入る前にロックする
	// 待ち時間も計測する
	{
		ProcessingTimeLogger log("CriticalSection1 Wait");
		mutexSection1.lock();
	}
	criticalSection1();
	mutexSection1.unlock();

	// クリティカルセクション２に入る前にロックする
	// 待ち時間も計測する
	{
		ProcessingTimeLogger log("CriticalSection2 Wait");
		mutexSection2.lock();
	}
	criticalSection2();
	mutexSection2.unlock();
}

void workerThread2()
{
	ProcessingTimeLogger workerlog("WorkerThread2");

	// クリティカルセクション２に入る前にロックする
	// 待ち時間も計測する
	{
		ProcessingTimeLogger log("CriticalSection2 Wait");
		mutexSection2.lock();
	}
	criticalSection2();
	mutexSection2.unlock();

	// クリティカルセクション１に入る前にロックする
	// 待ち時間も計測する
	{
		ProcessingTimeLogger log("CriticalSection1 Wait");
		mutexSection1.lock();
	}
	criticalSection1();
	mutexSection1.unlock();
}

int main()
{
	PerformanceLogger::getInstance()->startLogger("logTest.csv");

	ProcessingTimeLogger log("main");

	// スレッドで処理を行う。
	Sleep(10);
	std::thread threadA(workerThread1);
	std::thread threadB(workerThread2);
	std::thread threadC(workerThread1);
	std::thread threadD(workerThread2);
	Sleep(10);
	std::thread threadE(workerThread1);
	std::thread threadF(workerThread1);
	std::thread threadG(workerThread2);

	// スレッドの終了を待つ。
	threadA.join();
	threadB.join();
	threadC.join();
	threadD.join();
	threadE.join();
	threadF.join();
	threadG.join();

	return 0;
}