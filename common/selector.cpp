//
// selector.cpp
//

#include "selector.h"
#include "network.h"
#include "os.h"
#include "osthread.h"
#include "osthreadlock.h"

Selector::Selector()
{
}

Selector::~Selector()
{
	RemoveAll();
}

bool Selector::Add(Socket* pSock)
{
	if(pSock == NULL)
    { 
        return false;
    }

	m_pLock.Lock();

	m_pList.push_back(pSock);

	m_pLock.UnLock();

	return true;
}

bool Selector::Remove(Socket* pSock, bool bDelete)
{
	if(pSock == NULL)	
    {
        return false; 
    }

	m_pLock.Lock();

	m_pList.remove((void*)pSock);
	if(bDelete == true)
	{
		delete pSock;
		pSock = NULL;
	}

	m_pLock.UnLock();

	return true;
}

bool Selector::RemoveAll(bool bDelete)
{
	Socket* pSock = NULL;

	m_pLock.Lock();

	while(m_pList.size() > 0)
	{		
		pSock = (Socket*)m_pList.front();

		if(pSock != NULL && bDelete == TRUE )
		{
			delete pSock;
			pSock = NULL;
		}

		m_pList.pop_front();
	}

	m_pLock.UnLock();

	return true;
}

void Selector::DoSelect()
{
	fd_set ReadFds, WriteFds, ExceptFds;
	FD_ZERO(&ReadFds);
	FD_ZERO(&WriteFds);
	FD_ZERO(&ExceptFds);

	struct timeval Timeout;
	Timeout.tv_sec = 0;
	Timeout.tv_usec	= SOCKET_SELECT_TIME_OUT_MICROSECONDS;

#if defined(_OS_LINUX)
	SOCKET hMax = INVALID_SOCKET;
#endif

	m_pLock.Lock();

	for(CPointList::iterator it = m_pList.begin(); it != m_pList.end(); it++ )
	{		
		Socket* pSock = (Socket*)(*it);

		if(pSock != NULL)
		{
			FD_SET(pSock->fd(),&ReadFds);
			FD_SET(pSock->fd(),&WriteFds);
			FD_SET(pSock->fd(),&ExceptFds);
#if defined(_OS_LINUX)
			if(pSock->fd() > hMax)	
            { 
                hMax = pSock->fd();
            }
#endif
		}
	}

	m_pLock.UnLock();

#if defined(_OS_WINDOWS)
	int nSelectRet = select(0, &ReadFds, &WriteFds, &ExceptFds, &Timeout);
#elif defined(_OS_LINUX)
	int nSelectRet = select((hMax + 1), &ReadFds, &WriteFds, &ExceptFds, &Timeout);
#endif

	if(nSelectRet != SOCKET_ERROR)
	{
		CPointList tempList;

		m_pLock.Lock();

		for(CPointList::iterator it = m_pList.begin(); it != m_pList.end(); it++)
		{		
			Socket* pSock = (Socket*)(*it);
			if(pSock != NULL )
			{
				tempList.push_back(pSock);
			}
		}

		m_pLock.UnLock();

		for(CPointList::iterator it = tempList.begin(); it != tempList.end(); it++)
		{		
			Socket* pSock = (Socket*)(*it);

			if(pSock != NULL)
			{
				bool bSuccess = true;
                if(bSuccess && pSock != NULL && FD_ISSET(pSock->fd(), &ReadFds))
				{
					if(pSock->state() == kListening)
                    {
						bSuccess = pSock->OnAccept();
                        
                        if(bSuccess == false)
                        {
                            pSock->OnDropped(kAcceptError);
                        }
                    }
                    else if(pSock->state() == kConnected)
                    {
                        bSuccess = pSock->OnRead();
                        
                        if(bSuccess == false)
                        {
                            pSock->OnDropped(kReadError);
                        }
                    }
                }
                
				if(bSuccess && pSock != NULL && FD_ISSET(pSock->fd(), &WriteFds))
				{
					if(pSock->state() == kConnecting)
					{
						pSock->OnConnected();
					}
					else if(pSock->state() == kConnected)
					{
						bSuccess = pSock->OnWrite();
                        
						if(bSuccess == false)
                        {
							pSock->OnDropped(kWriteError);
                        }
					}
				}
			}
		}
	}
}

void Selector::OnRun()
{

}
