//
// Thread Lock
//

#include "osthreadlock.h"

OSThreadLock::OSThreadLock()
{
	InitialLock();
}

OSThreadLock::~OSThreadLock()
{
	UnitialLock();
}

void OSThreadLock::Lock()
{
#if _OS_WINDOWS
	EnterCriticalSection(&m_pSection);
#elif _OS_LINUX
	int nError = pthread_mutex_lock(&m_pSection);
	if(nError != 0)
	{
		printf("pthread_mutex_lock error : %d\n", nError);
	}
#endif
}

void OSThreadLock::UnLock()
{
#if _OS_WINDOWS
	LeaveCriticalSection(&m_pSection);
#elif _OS_LINUX
	int nError = pthread_mutex_unlock(&m_pSection);
	if(nError != 0)
	{
		printf("pthread_mutex_unlock error : %d\n", nError);
	}
#endif
}

void OSThreadLock::InitialLock()
{
#if _OS_WINDOWS
	InitializeCriticalSection(&m_pSection);
#elif _OS_LINUX
	pthread_mutex_init(&m_pSection, NULL);
#endif
}

void OSThreadLock::UnitialLock()
{
#if _OS_WINDOWS
	DeleteCriticalSection(&m_pSection);
#elif _OS_LINUX
	int nError = pthread_mutex_destroy(&m_pSection);
	if(nError != 0)
	{
		printf("pthread_mutex_destroy error : %d\n", nError);
	}
#endif
}
