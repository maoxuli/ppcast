//
// peerclientmgr.cpp
//

#include "peerclientmgr.h"
#include "peerclient.h"
#include "logger.h"
#include "settings.h"
#include "os.h"

PeerClientMgr::PeerClientMgr()
{
	m_nRunCookie = 0;	
}

PeerClientMgr::~PeerClientMgr()
{

}

void PeerClientMgr::AddClient(PeerClient* pClient)
{
    assert(pClient->fd() != INVALID_SOCKET && pClient->state() == kConnected);
	Add( pClient );
	//theLogger.Message(MSG_DEBUG,"Add One Peer Client %s", pPeerClient->m_RemoteHost.GetIPAddressString());
}

void PeerClientMgr::RemoveClient(PeerClient* pClient)
{
    Remove( pClient );
	//theLogger.Message(MSG_DEBUG,"Remove One Peer Client %s", pPeerClient->m_RemoteHost.GetIPAddressString());
}

PeerClient* PeerClientMgr::GetNextClient(UInt32L nCookie)
{
	PeerClient* pClient = NULL;
	BOOL bFound = FALSE;

	m_pLock.Lock();
	
	for(CPointList::iterator it = m_pList.begin(); it != m_pList.end(); it++)
	{
		pClient = (PeerClient*)(*it);

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

void PeerClientMgr::RunClients()
{
    m_nRunCookie++;

    while(TRUE)
    {
		PeerClient* pClient = GetNextClient(m_nRunCookie);
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

void PeerClientMgr::OnRun()
{
    while(m_bThread)
    {
		DoSelect();		
        RunClients();
		OSSleep(THREAD_INTERVAL_MILLISECONDS);
    }
}
