//
// selector
//

#ifndef __SELECTOR_H__
#define	__SELECTOR_H__

#include "osthread.h"
#include "osthreadlock.h"

class Socket;

class Selector : public OSThread
{
public:
	Selector();
	virtual ~Selector();

	bool Add(Socket* pSock);
	bool Remove(Socket* pSock, bool bDelete = false);
	bool RemoveAll(bool bDelete = true);

	void DoSelect();
    
	virtual void OnRun();

protected:
	OSThreadLock m_pLock;
    CPointList m_pList;
};

//extern CTcpListeners TcpListeners;

#endif//__TCPLISTENERS_H__
