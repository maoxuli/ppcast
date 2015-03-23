//
// rtspserver.h
//

#ifndef __RTSP_SERVER_H__
#define __RTSP_SERVER_H__

#include "selector.h"

class ChannelMgr;

// RTSP server is a running thread 
// select on rtsp listener and rtsp connections
// Rtsp session manager
// Also handle rtsp requests received by multiple rtsp connections
class RtspServer : public Selector
{
public:
    RtspServer(ChannelMgr* channelMgr);
    virtual ~RtspServer();
    
    bool Start();
    bool Stop();
    
protected:
    // Override Selector::OnRun()
    // Thread running
    virtual bool OnRun();
    
protected:
    //
    friend class RtspListener;
    friend class RtspConnection;
    
    // Listener
    RtspListener* _listener;
    std::list<RtspConnection*> _connections;
    
    // Receive a connection request from rtsp client
    onConnection(RtspConnection* conn);
    
    // Receive a rtsp request from a rtsp connection
    void onRequest(RtspRequest* msg, RtspConnection* conn);

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
