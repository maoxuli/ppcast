//
// netserver.h
//

#ifndef _NETSERVER_H__
#define	_NETSERVER_H__

#include "tcplistener.h"

class PeerClientMgr;

// A NetServer is listeningsocket
// Runing in a selecting thread (NetServerMgr)
class NetServer : public TcpListener
{
public:
    NetServer(const Endpoint& endpoint);
    virtual ~NetServer();

public:
    // Start
	bool Start();
    
    // Peer count count
    UInt32L GetClientCount();

protected:
    // Override from TcpListener
	bool OnAccept();
	void CreateClient(SOCKET hSocket);
    
private:
    Endpoint _endpoint;
    
    // Threads for tcp transport sockets (PeerClientMgr)
    // Determined by processor count
    PeerClientMgr** _clientMgrList;
    unsigned short _processorCount;
    
    UInt32L				m_nAcceptedClientsCount;

    UInt16              m_nProcessor;
	CSTLString			m_sLocalURL;
    UInt32L			    m_nSequence;
};

#endif 
