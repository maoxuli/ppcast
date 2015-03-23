//
// rtsplistener.h
//

#ifndef __RTSP_LISTENER_H__
#define __RTSP_LISTENER_H__

#include <rtsp/RtspMessage.h>
#include <rtsp/RtspConnection.h>
#include "tcplistener.h"

//
// RTSP listener is extension of a TCP listener
// Handle RTSP connection requests from multiple RtspConnections
// 
class RtspListener : public TcpListener
{
public:
    
    RtspListener(const Endpoint& endpoint, RtspServer* server);
    virtual ~RtspListener();
    
    bool Start();
    
    // Receive a request from a RtspConnection
    void onRequest(RtspRequest* msg, RtspConnection* conn);
    
protected:
    
    // Override to clear sessions when shutdown
    virtual void doShutdown();
    
    // Override to run sessions
    virtual bool doRun();
    
    // Override to create RtspConnection
    virtual TcpConnection* createConnection(SOCKET fd);
    
protected:

    // RTSP serve handle RTSP messages received over multiple RTSP connections
    void OnDescribeRequest(RtspRequest* msg, RtspConnection* conn);
    void OnOptionsRequest(RtspRequest* msg, RtspConnection* conn);
    void OnSetupRequest(RtspRequest* msg, RtspConnection* conn);
    void OnGetParamRequest(RtspRequest* msg, RtspConnection* conn);
    void OnSetParamRequest(RtspRequest* msg, RtspConnection* conn);
    void OnPlayRequest(RtspRequest* msg, RtspConnection* conn);
    void OnPauseRequest(RtspRequest* msg, RtspConnection* conn);
    void OnTeardownRequest(RtspRequest* msg, RtspConnection* conn);

    // RtspServer need to know some media info
    // that is bound to medias managed by application
    virtual std::string mediaSDP(const std::string& mid) = 0;

protected:
    
    // Session based stream managements
    std::vector<RtspSession*> _sessions;

    // Session ID, automatically increased with new session
    unsigned int _sid;

    // A session is identified with sid
    RtspSession* findSession(const std::string& sid);

    // A RTSP server is resided between media and player
    // A session is linked to a player with session ID
    // A session is linked to a media with media ID
    void removeSession(const std::string& sid);
    void removeMedia(const std::string& mid);

    // RTSPSession is linked to client player with RTSP session ID (sid)
    // RTSPSession is linked to local media with media ID (mid)
    // Medias are managed by application
    virtual RtspSession* createSession(const std::string& sid, const std::string& mid) = 0;
};

#endif 
