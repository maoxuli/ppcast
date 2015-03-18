//
// Thread
//

#ifndef __OS_THREAD_H__
#define __OS_THREAD_H__

#include "os.h"

class OSThread
{
public:
	OSThread();
	virtual ~OSThread();

public:
	bool StartThread();
    
	virtual	UInt32 StopThread(UInt32 nTimeOut = THREAD_EXIT_TIMEOUT_MILLISECONDS);
	virtual	void OnRun();

protected:
	UInt32 StopThread(bool bForce);

protected:

#if _OS_WINDOWS
	HANDLE	m_hThread;
	UInt32	m_nThreadID;
	UInt32	m_dwExitCode;
#elif _OS_LINUX
	pthread_t m_hThread;
	pthread_attr_t m_aThread;
#endif

	volatile bool m_bThread;	
};

#endif
