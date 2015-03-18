//
// netservermgr.cpp
//

#include "netservermgr.h"
#include "netserver.h"
#include "logger.h"
#include "settings.h"

NetServerMgr theServerMgr;

NetServerMgr::NetServerMgr()
{
	m_pServer = NULL;
}

NetServerMgr::~NetServerMgr()
{
    
}

bool NetServerMgr::Start()
{
    Endpoint endpoint; 
    endpoint.Set(theSettings.System.Host.c_str(), theSettings.System.Port);
    
	m_pServer = new NetServer(endpoint);
	if(m_pServer->Start() == false)
    {
        return false;
    }
    
	Add(m_pServer);

	if(StartThread() == false)	
    {
        return false;
    }

	return true;
}

void NetServerMgr::Stop()
{
	StopThread();
}

void NetServerMgr::OnRun()
{
	UInt32L tLast, tNow;
	tLast = tNow = OSMilliSeconds();

	while(m_bThread)
	{
		DoSelect();

		tNow = OSMilliSeconds();
		if(tNow - tLast > 30000)
		{
			tLast = tNow;
			theLogger.Message(MSG_DEBUG,"PeerClient Number = %d\r\n", m_pServer->GetClientCount());
		}
        
		OSSleep(THREAD_INTERVAL_MILLISECONDS);
	}
}
