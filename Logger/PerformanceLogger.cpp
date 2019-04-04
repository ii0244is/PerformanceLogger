
#include "PerformanceLogger.h"

#include <thread>

/////////////////////////////////////////////////////////////////////////
//  PerfomanceCounter
/////////////////////////////////////////////////////////////////////////

PerformanceCounter::PerformanceCounter(void)
{
	resetTimer();
}

PerformanceCounter::~PerformanceCounter(void)
{
}

void PerformanceCounter::resetTimer()
{
	QueryPerformanceFrequency(&m_freqency);
	QueryPerformanceCounter(&m_startTime);
}

double PerformanceCounter::getTime()
{
	double time = 0.0;
	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);

	time = (double)( ( currentTime.QuadPart - m_startTime.QuadPart ) * 1000000 / m_freqency.QuadPart );
	return time;
}


/////////////////////////////////////////////////////////////////////////
//  ProcessingTimeLogger
/////////////////////////////////////////////////////////////////////////

ProcessingTimeLogger::ProcessingTimeLogger(const char* userComment)
{
	m_userComment = userComment;
	m_performace.resetTimer();
	m_startTime = PerformanceLogger::getInstance()->getTime();
	PerformanceLogger::getInstance()->outputLog(
		m_userComment.c_str(), "start", m_startTime, 0);
}

ProcessingTimeLogger::~ProcessingTimeLogger()
{
	PerformanceLogger::getInstance()->outputLog(
		m_userComment.c_str(), "stop", m_startTime, m_performace.getTime());
}


/////////////////////////////////////////////////////////////////////////
//  PerformanceLogger
/////////////////////////////////////////////////////////////////////////

PerformanceLogger::PerformanceLogger() : 
m_bLoggerActive( false ),
m_fp( NULL )
{
}

PerformanceLogger::~PerformanceLogger(void)
{
	if( m_bLoggerActive && m_fp != NULL )
	{
		stopLogger();
	}
}

PerformanceLogger* PerformanceLogger::getInstance()
{
	static PerformanceLogger s_logger;
	return &s_logger;
}

void PerformanceLogger::startLogger(const char* fileName)
{
	if( !m_bLoggerActive )
	{
		m_bLoggerActive = true;
		m_performace.resetTimer();
		fopen_s( &m_fp, fileName, "w" );
		fprintf_s( m_fp, "Time[us],ThreadID,UserComment1,UserComment2,val1,val2\n" );
	}
}

void PerformanceLogger::stopLogger()
{
	if( m_bLoggerActive )
	{
		m_bLoggerActive = false;
		fclose( m_fp );
		m_fp = NULL;
	}
}

double PerformanceLogger::getTime()
{
	return m_performace.getTime();
}

void PerformanceLogger::outputLog( const char* userComment1, const char* userComment2 )
{
	if( m_bLoggerActive && m_fp != NULL )
	{
		char log[512];
		sprintf_s( log, "%f,%d,%s,%s,0,0\n", 
			m_performace.getTime(), std::this_thread::get_id(), userComment1, userComment2);

		m_mutexLogger.lock();
		fprintf_s( m_fp, log );
		m_mutexLogger.unlock();
	}
}

void PerformanceLogger::outputLog(const char* userComment1, const char* userComment2, double val1, double val2)
{
	if (m_bLoggerActive && m_fp != NULL)
	{
		char log[512];
		sprintf_s(log, "%f,%d,%s,%s,%f,%f\n",
			m_performace.getTime(), std::this_thread::get_id(), 
			userComment1, userComment2, val1, val2);

		m_mutexLogger.lock();
		fprintf_s(m_fp, log);
		m_mutexLogger.unlock();
	}
}
