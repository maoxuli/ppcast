//
// netservermgr.h
//

#ifndef _NET_SERVER_MGR_H__
#define	_NET_SERVER_MGR_H__

#include "selector.h"

class NetServer; 

// A selector is a thread 
// A NetServerMgr is a single thread server
class NetServerMgr : public Selector
{
public:
	NetServerMgr();
	virtual ~NetServerMgr();

	bool Start();
	void Stop();

    // Thread running
	void OnRun();

protected:
    // A listing socket
	NetServer* m_pServer;
};

// The system only support one thread and one listing socket
extern NetServerMgr theServerMgr;

#endif 
