//
// sourceclientmgr.h
//

#ifndef	__SOURCE_CLIENT_MGR_H__
#define __SOURCE_CLIENT_MGR_H__

#include "selector.h"

class SourceClient;

// A selector is a thread 
// A SourceClientMgr is a thread manage tcp connections
class SourceClientMgr : public Selector
{
public:
    SourceClientMgr();
    ~SourceClientMgr();
        
    void AddClient(SourceClient* client);
    void RemoveClient(SourceClient* client);    
    inline size_t GetClientCount() 
    { 
        return m_pList.size(); 
    }
    
protected:
    virtual void OnRun();
    
    void RunClients();
	SourceClient* GetNextClient(UInt32L nCookie);
    
    UInt32L m_nRunCookie;
};

extern SourceClientMgr theClientMgr;

#endif

