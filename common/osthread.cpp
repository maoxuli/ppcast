//
// Thread
//

#include "osthread.h"
#include "os.h"

#if _OS_WINDOWS
UInt32 __stdcall ThreadStart(void* pParam)
{
	OSThread* pThread = (OSThread*)pParam;
	pThread->OnRun();
	//printf("thread return\n");
	return 0;
}
#elif _OS_LINUX
void* ThreadStart(void* pParam)
{
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	OSThread* pThread = (OSThread*)pParam;
	pThread->OnRun();
	//printf("thread return\n");
     return NULL;
}
#endif

OSThread::OSThread()
{
	m_bThread = FALSE;
#if _OS_WINDOWS
	m_hThread =	NULL;
	m_dwExitCode = 0;
	m_nThreadID = 0;
#endif
}

OSThread::~OSThread()
{
	StopThread();
}

bool OSThread::StartThread()
{
	m_bThread = true;

#if _OS_WINDOWS
	HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, &ThreadStart, (void*)this, 0, &m_nThreadID);
	if(hThread != NULL)
	{
		m_hThread = hThread;	
	}
	else
	{
		m_bThread = false;
	}
#elif _OS_LINUX
	pthread_attr_init(&m_aThread);
	pthread_attr_setscope(&m_aThread, PTHREAD_SCOPE_SYSTEM);
	int nError = pthread_create(&m_hThread, &m_aThread, &ThreadStart, (void*)this);
	if(nError != 0)
	{
		m_bThread = false;
		printf("pthread_create error : %d\n", nError);
	}
#endif

	return m_bThread;
}

UInt32 OSThread::StopThread(UInt32 nTimeOut)
{
	StopThread(false);
#if _OS_WINDOWS
	WaitForSingleObject(m_hThread , nTimeOut);
#elif _OS_LINUX
	OSSleep(nTimeOut);
#endif
	return StopThread(true);
}

UInt32 OSThread::StopThread(bool bForce)
{
#if _OS_WINDOWS
	if(m_hThread != NULL)
	{
		if(m_bThread == true)
        {
			m_bThread = false;		//first, try to stop the thread nice
        }

		GetExitCodeThread(m_hThread, (UInt32L*)&m_dwExitCode);

		if(m_dwExitCode == STILL_ACTIVE && bForce == TRUE)
		{
			TerminateThread(m_hThread, UInt32L(-1));
			m_hThread = NULL;
		}
	}

	return m_dwExitCode;

#elif _OS_LINUX
	if(m_bThread == true)
    {
		m_bThread = false;
    }

	//if ( bForce == true )
	//{
	//	int nError = pthread_cancel( m_bThread );
	//	if ( nError != 0 )
	//	{
	//		printf( "pthread_cancel error : %d\n", nError);
	//	}
	//}

	return 0;
#endif
}

//
//	
//
void OSThread::OnRun()
{
	return;
}

