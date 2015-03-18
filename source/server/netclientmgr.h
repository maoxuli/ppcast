//
// netclientmgr.h
//

#ifndef	__NET_CLIENT_MGR_H__
#define __NET_CLIENT_MGR_H__

#include "selector.h"

class NetClient;

// A selector is a thread 
// A PeerClientMgr is a thread manage tcp connections
class NetClientMgr : public Selector
{
public:
    NetClientMgr();
    ~NetClientMgr();
        
    void AddClient(NetClient* client);
    void RemoveClient(NetClient* client);    
    inline size_t GetClientCount() 
    { 
        return m_pList.size(); 
    }
    
protected:
    virtual void OnRun();
    
    void RunClients();
	NetClient* GetNextClient(UInt32L nCookie);
    
    UInt32L m_nRunCookie;
};

#endif

