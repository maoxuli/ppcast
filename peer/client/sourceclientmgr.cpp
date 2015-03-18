//
// SourceClientMgr.cpp
//

#include "SourceClientMgr.h"
#include "sourceclient.h"
#include "logger.h"

#include "os.h"

SourceClientMgr theClientMgr;

SourceClientMgr::SourceClientMgr()
{
	m_nRunCookie = 0;	
}

SourceClientMgr::~SourceClientMgr()
{

}

void SourceClientMgr::AddClient(SourceClient* pClient)
{
    assert(pClient->fd() != INVALID_SOCKET && pClient->state() == kConnecting);
	Add( pClient );
	//theLogger.Message(MSG_DEBUG,"Add One Peer Client %s", pPeerClient->m_RemoteHost.GetIPAddressString());
}

void SourceClientMgr::RemoveClient(SourceClient* pClient)
{
    Remove( pClient );
	//theLogger.Message(MSG_DEBUG,"Remove One Peer Client %s", pPeerClient->m_RemoteHost.GetIPAddressString());
}

SourceClient* SourceClientMgr::GetNextClient(UInt32L nCookie)
{
	SourceClient* pClient = NULL;
	BOOL bFound = FALSE;

	m_pLock.Lock();
	
	for(CPointList::iterator it = m_pList.begin(); it != m_pList.end(); it++)
	{
		pClient = (SourceClient*)(*it);

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

void SourceClientMgr::RunClients()
{
    m_nRunCookie++;

    while(TRUE)
    {
		SourceClient* pClient = GetNextClient(m_nRunCookie);
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

void SourceClientMgr::OnRun()
{
    while(m_bThread)
    {
		DoSelect();		
        RunClients();
		OSSleep(THREAD_INTERVAL_MILLISECONDS);
    }
}
