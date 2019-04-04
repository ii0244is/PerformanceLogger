#pragma once

#include <Windows.h>
#include <stdio.h>
#include <mutex>
#include <string>

/////////////////////////////////////////////////////////////////////////
//  PerfomanceCounter
/////////////////////////////////////////////////////////////////////////

class PerformanceCounter
{
public:
	PerformanceCounter(void);
	~PerformanceCounter(void);

	void   resetTimer();
	double getTime();

private:
	LARGE_INTEGER m_freqency;
	LARGE_INTEGER m_startTime;
};

/////////////////////////////////////////////////////////////////////////
//  ProcessingTimeLogger
/////////////////////////////////////////////////////////////////////////

class ProcessingTimeLogger
{
public:
	ProcessingTimeLogger(const char* userComment);
	~ProcessingTimeLogger();

private:
	std::string m_userComment;
	double m_startTime;
	PerformanceCounter m_performace;
};


/////////////////////////////////////////////////////////////////////////
//  PerformanceLogger
/////////////////////////////////////////////////////////////////////////

class PerformanceLogger
{
public:
	static PerformanceLogger* getInstance();
	void   startLogger(const char* fileName );
	void   stopLogger();
	double getTime();
	void   outputLog(const char* userComment1, const char* userComment2 = "");
	void   outputLog(const char* userComment1, const char* userComment2, double val1, double val2 );

private:
	PerformanceLogger();
	~PerformanceLogger(void);

	bool  m_bLoggerActive;
	FILE* m_fp;
	PerformanceCounter m_performace;
	std::mutex m_mutexLogger;
};
