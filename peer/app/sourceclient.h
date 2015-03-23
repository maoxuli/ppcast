
#ifndef	__SOURCE_CLIENT_H__
#define	__SOURCE_CLIENT_H__

#include "tcpconnection.h"

class PPPacket;

class SourceClient: public TcpConnection
{
public:
    SourceClient(const Endpoint& endpoint);
    virtual ~SourceClient();

    bool Initialize();
    void Destroy();

public:
    virtual bool OnRun();
    virtual bool OnConnected();
    virtual void OnDropped(int nError);
    virtual bool OnRead();
    
    bool SendJoinRequest();
    bool SendMetaRequest();
    bool SendDataRequest( size_t nMin, size_t nMax );
    bool SendResetRequest();

    bool OnPacket( PPPacket * pPacket);
    bool OnAllowPacket( PPPacket * pPacket); 
    bool OnRejectPacket( PPPacket * pPacket); 
    bool OnMetaPacket( PPPacket * pPacket);
    bool OnDataPacket( PPPacket * pPacket);
    bool OnDataFailPacket( PPPacket * pPacket);  
    
private:
    Endpoint _endpoint;
    unsigned int m_nRunCookie;
};

#endif 
