//
// rtspserver.h
//

#ifndef __RTSP_SERVER_H__
#define __RTSP_SERVER_H__

#include "selector.h"
#include "rtspmessage.h"

class ChannelMgr;
class RtspListener;
class RtspConnection;
class RtspSession;

// RTSP server is a running thread 
// select sockets of rtsp listener and rtsp connections
// accept connections and handle rtsp requests
// session manager, one session per client

class RtspServer : public Selector
{
public:
    RtspServer();
    virtual ~RtspServer();
    
    bool Start();
    void Stop();
    
    // From selector
    // Running sockets and customized task
    virtual void OnRun();
    
protected:
    // Accept connections with listener
    RtspListener* _listener;
    
    // Accepted connections
    std::list<RtspConnection*> _connections;
    
    // Call by listener when a connection is accepted
    void OnConnection(SOCKET fd);
    friend class RtspListener;
    
    // Call by connection when a rtsp request is received
    void OnRequest(RtspRequest* msg, RtspConnection* conn);
    friend class RtspConnection;

    // handle RTSP messages
    void OnDescribeRequest(RtspRequest* msg, RtspConnection* conn);
    void OnOptionsRequest(RtspRequest* msg, RtspConnection* conn);
    void OnSetupRequest(RtspRequest* msg, RtspConnection* conn);
    void OnGetParamRequest(RtspRequest* msg, RtspConnection* conn);
    void OnSetParamRequest(RtspRequest* msg, RtspConnection* conn);
    void OnPlayRequest(RtspRequest* msg, RtspConnection* conn);
    void OnPauseRequest(RtspRequest* msg, RtspConnection* conn);
    void OnTeardownRequest(RtspRequest* msg, RtspConnection* conn);

protected:
    // Session based stream managements
    std::vector<RtspSession*> _sessions;

    // Session ID, automatically increased with new session
    unsigned int _sid;

    // A session is identified with sid
    RtspSession* findSession(const std::string& sid);
    void removeSession(const std::string& sid);

protected:
    // A new session linked to a media channel
    // Registered to a channel
    RtspSession* createSession(const std::string& media);
};

extern RtspServer theRtspServer;

#endif 
