//
// logger.cpp
//

#include "logger.h"

#include "os.h"
#include "osthreadlock.h"

Logger theLogger;

char Logger::LogBuffer[1024];

Logger::Logger()
{
}

Logger::~Logger()
{
}

void Logger::Message(int nType, const char* pszFormat, ...)
{
	m_pLock.Lock();

	va_list pArgs;
	memset( LogBuffer, 0, 1024 );

	va_start(pArgs, pszFormat);
	vsprintf( LogBuffer, pszFormat, pArgs);
	va_end( pArgs );

	PrintMessage( nType, LogBuffer );
	LogMessage( LogBuffer );

	m_pLock.UnLock();
}

void Logger::PrintMessage(int nType, const char* pszLog)
{
	printf( "%s\r\n", pszLog );
}

void Logger::LogMessage(const char* pszLog)
{
	FILE *pFile;
	char sTime[64];
	time_t currentTime;
	struct tm *pNow;

    // Todo: log file configuartion
	if((pFile = fopen("ppsource.log", "a+")) == NULL)
    {
		if((pFile = fopen("ppsource.log", "w+")) == NULL)
        {
			return;
        }
    }

	currentTime = time(0);
	pNow = localtime(&currentTime);

	memset(sTime, 0, 64);
	sprintf(sTime, "[%02d:%02d:%02d] ",pNow->tm_hour,pNow->tm_min,pNow->tm_sec);

	fprintf(pFile, sTime);
	fprintf(pFile, pszLog);
	fprintf(pFile, "\r\n");

	fclose(pFile);
}

