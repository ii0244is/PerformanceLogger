
#include "../Logger/PerformanceLogger.h"

std::mutex mutexSection1;
std::mutex mutexSection2;

void criticalSection1()
{
	ProcessingTimeLogger log("CriticalSection1");

	// ���Ӗ��Ȍv�Z�����Ď��Ԃ��҂��B
	// �R���p�C�����A�œK���I�v�V������ OFF �ɂ��Ă����B
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

	// ���Ӗ��Ȍv�Z�����Ď��Ԃ��҂��B
	// �R���p�C�����A�œK���I�v�V������ OFF �ɂ��Ă����B
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

	// �N���e�B�J���Z�N�V�����P�ɓ���O�Ƀ��b�N����
	// �҂����Ԃ��v������
	{
		ProcessingTimeLogger log("CriticalSection1 Wait");
		mutexSection1.lock();
	}
	criticalSection1();
	mutexSection1.unlock();

	// �N���e�B�J���Z�N�V�����Q�ɓ���O�Ƀ��b�N����
	// �҂����Ԃ��v������
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

	// �N���e�B�J���Z�N�V�����Q�ɓ���O�Ƀ��b�N����
	// �҂����Ԃ��v������
	{
		ProcessingTimeLogger log("CriticalSection2 Wait");
		mutexSection2.lock();
	}
	criticalSection2();
	mutexSection2.unlock();

	// �N���e�B�J���Z�N�V�����P�ɓ���O�Ƀ��b�N����
	// �҂����Ԃ��v������
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

	// �X���b�h�ŏ������s���B
	Sleep(10);
	std::thread threadA(workerThread1);
	std::thread threadB(workerThread2);
	std::thread threadC(workerThread1);
	std::thread threadD(workerThread2);
	Sleep(10);
	std::thread threadE(workerThread1);
	std::thread threadF(workerThread1);
	std::thread threadG(workerThread2);

	// �X���b�h�̏I����҂B
	threadA.join();
	threadB.join();
	threadC.join();
	threadD.join();
	threadE.join();
	threadF.join();
	threadG.join();

	return 0;
}