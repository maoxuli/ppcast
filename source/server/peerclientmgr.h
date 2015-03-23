//
// peerclientmgr.h
//

#ifndef	__PEER_CLIENT_MGR_H__
#define __PEER_CLIENT_MGR_H__

#include "selector.h"

class PeerClient;

// A selector is a thread 
// A PeerClientMgr is a thread manage tcp connections
class PeerClientMgr : public Selector
{
public:
    PeerClientMgr();
    ~PeerClientMgr();
        
    void AddClient(PeerClient* client);
    void RemoveClient(PeerClient* client);    
    inline size_t GetClientCount() 
    { 
        return m_pList.size(); 
    }
    
protected:
    virtual void OnRun();
    
    void RunClients();
	PeerClient* GetNextClient(UInt32L nCookie);
    
    UInt32L m_nRunCookie;
};

#endif

