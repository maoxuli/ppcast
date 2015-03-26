//
// rtsplistener.h
//

#ifndef __RTSP_LISTENER_H__
#define __RTSP_LISTENER_H__

#include "tcplistener.h"

class RtspServer;

// TCP Listener for RTSP server
// Accept connection from RTSP clients 
// and pass the connections to RTSP server

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
