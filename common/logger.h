//
// logger.h
//

#ifndef __LOGGER_H__
#define __LOGGER_H__

#include "osthreadlock.h"

// Log message types
#define MSG_DEFAULT		0
#define MSG_SYSTEM		1
#define MSG_ERROR		2
#define MSG_DEBUG		3
#define MSG_TEMP		4

class Logger
{
public:
	Logger();
    virtual ~Logger();

public:
	void Message(int nType, const char* pszFormat, ...);
	void PrintMessage(int nType, const char* pszLog);
	void LogMessage(const char* pszLog);
    
private:
    static char	LogBuffer[1024];
	OSThreadLock m_pLock;
};

extern Logger theLogger;

#endif
