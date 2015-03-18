//
// netclientmgr.cpp
//

#include "netclientmgr.h"
#include "netclient.h"
#include "logger.h"
#include "settings.h"

#include "os.h"

NetClientMgr::NetClientMgr()
{
	m_nRunCookie = 0;	
}

NetClientMgr::~NetClientMgr()
{

}

void NetClientMgr::AddClient(NetClient* pClient)
{
    assert(pClient->fd() != INVALID_SOCKET && pClient->state() == kConnected);
	Add( pClient );
	//theLogger.Message(MSG_DEBUG,"Add One Peer Client %s", pPeerClient->m_RemoteHost.GetIPAddressString());
}

void NetClientMgr::RemoveClient(NetClient* pClient)
{
    Remove( pClient );
	//theLogger.Message(MSG_DEBUG,"Remove One Peer Client %s", pPeerClient->m_RemoteHost.GetIPAddressString());
}

NetClient* NetClientMgr::GetNextClient(UInt32L nCookie)
{
	NetClient* pClient = NULL;
	BOOL bFound = FALSE;

	m_pLock.Lock();
	
	for(CPointList::iterator it = m_pList.begin(); it != m_pList.end(); it++)
	{
		pClient = (NetClient*)(*it);

		if(pClient->m_nRunCookie != nCookie)
		{
			pClient->m_nRunCookie = nCookie;
			bFound = TRUE;
			break;
		}
	}

	m_pLock.UnLock();
	return bFound == TRUE ? pClient : NULL;
}

void NetClientMgr::RunClients()
{
    m_nRunCookie++;

    while(TRUE)
    {
		NetClient* pClient = GetNextClient(m_nRunCookie);
		if(!pClient) 
        {
            break;
        }

		if(pClient->OnRun() == false) 
		{
			RemoveClient(pClient);
			delete pClient;
		}
    }
}

void NetClientMgr::OnRun()
{
    while(m_bThread)
    {
		DoSelect();		
        RunClients();
		OSSleep(THREAD_INTERVAL_MILLISECONDS);
    }
}
