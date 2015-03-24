//
// rtsplistener.h
//

#ifndef __RTSP_LISTENER_H__
#define __RTSP_LISTENER_H__

#include "tcplistener.h"

class RtspServer;

//
// RTSP listener is extension of a TCP listener
// Handle RTSP connection requests from multiple RtspConnections
// 
class RtspListener : public TcpListener
{
public:
    
    RtspListener(RtspServer* server);
    virtual ~RtspListener();
    
    virtual bool OnAccept();
        
protected:
    RtspServer* _server;
};

#endif 
