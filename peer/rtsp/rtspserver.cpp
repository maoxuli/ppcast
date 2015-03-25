//
// rtspserver.cpp
//

#include "rtspserver.h"
#include "rtsplistener.h"
#include "rtspconnection.h"
#include "rtspsession.h"
#include "rtspmessage.h"
#include "channelmgr.h"
#include "channelurl.h"
#include <sstream>

RtspServer::RtspServer(ChannelMgr* channelMgr)
: _channelMgr(channelMgr)
{
    assert(_channelMgr != NULL);
    _listener = NULL;
}

RtspServer::~RtspServer()
{
    if(_sessions.size() > 0)
    {
        std::cout << "Rtsp Server is not stoped before delete.\n";
        Stop();	
    }
    
    if(_listener != NULL)
    {
        delete _listener;
    }
}

bool RtspServer::Start()
{
    if(_listener == NULL)
    {
        _listener = new RtspListener(this);
    }
    
    assert(_listener != NULL);
    Endpoint endpoint;
    endpoint.Set("127.0.0.1", 1534);
    if(!_listener->DoBind(endpoint) || !_listener->DoListen(10))
    {
        return false;
    }
    
    Add(_listener);
    
    if(!StartThread())
    {
        return false;
    }
    
    return true;
}

void RtspServer::Stop()
{
    // Stop thread
    StopThread();
    
    // Clear RtspSessions
    for(std::vector<RtspSession*>::iterator it = _sessions.begin(); it != _sessions.end(); ++it)
    {
        RtspSession* p = *it;
        assert(p != NULL);;
        if(p != NULL)
        {
            p->close();
            delete p;
        }
    }
    _sessions.clear();
}

// Run sessions
void RtspServer::OnRun()
{
    while (m_bThread)
    {
        DoSelect();
        
        // Schedule streaming over sessions
        // If a session work failed, close the session
        for(std::vector<RtspSession*>::iterator it = _sessions.begin(); it != _sessions.end(); )
        {
            RtspSession* p = *it;
            assert(p != NULL);
            
            if(!p->run())
            {
                p->close();
                delete p;
                it = _sessions.erase(it);
            }
            else
            {
                ++it;
            }
        }   
    }
}

void RtspServer::OnConnection(SOCKET fd)
{
    std::cout << "RtspServer accept a connection.\n";
    assert(fd != INVALID_SOCKET);
    RtspConnection* conn = new RtspConnection(this);
    assert(conn != NULL);
    
    conn->AttachTo(fd);
    _connections.push_back(conn);
    
    Add(conn);
}

void RtspServer::removeSession(const std::string& sid)
{
    for(std::vector<RtspSession*>::iterator it = _sessions.begin(); it != _sessions.end(); )
    {
        RtspSession* p = *it;
        if(p != NULL && p->id() == sid)
        {
            p->close();
            delete p;
            it = _sessions.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

// Close all sessions related to the given media
void RtspServer::removeMedia(const std::string& mid)
{
    for(std::vector<RtspSession*>::iterator it = _sessions.begin(); it != _sessions.end(); )
    {
        RtspSession* p = *it;
        if(p != NULL && p->media() == mid)
        {
            p->close();
            delete p;
            it = _sessions.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

RtspSession* RtspServer::findSession(const std::string& sid)
{
    for(std::vector<RtspSession*>::iterator it = _sessions.begin(); it != _sessions.end(); )
    {
        RtspSession* p = *it;
        if(p != NULL && p->id() == sid)
        {
            return p;
        }
    }
    
    return NULL;
}

RtspSession* RtspServer::createSession(const std::string& mediaName)
{
    // Next session ID
    _sid ++;
    std::ostringstream oss;
    oss << _sid;
    std::string sid = oss.str();
    
    RtspSession* session = new RtspSession(sid, mediaName);
    assert(session != NULL);
    _sessions.push_back(session);
    
    return session;
}

// Handle a RTSP request message
void RtspServer::OnRequest(RtspRequest* request, RtspConnection* conn)
{
    std::cout << "On request\n";
    assert(request != NULL);
    assert(conn != NULL);
    std::string method = request->method();
    
    if(method == "OPTIONS") OnOptionsRequest(request, conn);
    else if(method == "DESCRIBE") OnDescribeRequest(request, conn);
    else if(method == "SETUP") OnSetupRequest(request, conn);
    else if(method == "GET_PARAMETER") OnGetParamRequest(request, conn);
    else if(method == "SET_PARAMETER") OnSetParamRequest(request, conn);
    else if(method == "PLAY") OnPlayRequest(request, conn);
    else if(method == "PAUSE") OnPauseRequest(request, conn);
    else if(method == "TEARDOWN") OnTeardownRequest(request, conn);
    else assert(false);
}

// Query supported commands
// Not affect session state
void RtspServer::OnOptionsRequest(RtspRequest* request, RtspConnection* conn)
{
    assert(request != NULL);
    assert(conn != NULL);
    
    // URL: rtsp:// 127.0.0.1:9960/movies/test.wmv?source=...&tracker=...&...
    ChannelUrl url(request->url());
    _channelMgr->StartChannel(url);
    
    RtspResponse response;
    response.setStatus(200);
    response.setHeader("CSeq", request->header("CSeq"));
    response.setHeader("Public", "OPTIONS, DESCRIBE, SETUP, TEARDOWN, PLAY, PAUSE, GET_PARAMETER, SET_PARAMETER");

    conn->SendResponse(&response);
}

// Query SDP of media
// Not affect session state
void RtspServer::OnDescribeRequest(RtspRequest* request, RtspConnection* conn)
{
    assert(request != NULL);
    assert(conn != NULL);
    
    // URL: rtsp:// 127.0.0.1:9960/movies/test.wmv?
    ChannelUrl url(request->url());
    Channel* channel = _channelMgr->GetChannel(url);
    
    // SDP
    assert(channel != NULL);
    std::string sdp = channel->GetSDP();
    if(sdp.empty())
    {
        // Let rtsp client retry
        return;
    }
    
    // 
    std::ostringstream oss;
    oss << sdp.size();
    
    // Response with sdp
    RtspResponse response;
    response.setStatus(200);
    response.setHeader("CSeq", request->header("CSeq"));
    response.setHeader("Content-base", request->url());
    response.setHeader("Content-type","application/sdp");
    response.setHeader("Content-Length", oss.str());
    response.setBody(sdp.c_str(),sdp.size());
    
    conn->SendResponse(&response);
}

// Rtsp SETUP request
// Negotiation to establish a session and the streams in the session
void RtspServer::OnSetupRequest(RtspRequest* request, RtspConnection* conn)
{
    assert(request != NULL);
    assert(conn != NULL);
    
    // URL: rtsp: //127.0.0.1:9960/movies/test.wmv/[rtx/audio/video]
    std::string url = request->url();
    size_t pos = url.rfind("/");
    std::string streamName = url.substr(pos + 1);
    std::string mediaName = url.erase(pos);
    
    // At this point, 
    // if it is first setup command, should no session id assigned
    // otherwise, a session id should exist
    RtspSession* session = NULL;
    std::string sid = request->header("Session");
    if(!sid.empty())
    {
        session = findSession(sid);
        assert(session != NULL);
        assert(sid == session->id());
        assert(mediaName == session->media());
    }
    else
    {
        session = createSession(mediaName);
        assert(session != NULL);
        sid = session->id();
    }
    
    // Setup stream for session
    unsigned short serverPort = 9920;
    unsigned short  clientPort = 0;
    int nPorts = 0;
    std::string strTran = request->header( "Transport" );
    //CRequestTransportHdr rqtHdr(strTran);
    //rqtHdr.GetBasePort(&clientPort, &nPorts );
    
    //if( rqtHdr.CanUDP() )
    {
        // Over UDP
        session->setupStream(streamName, serverPort, clientPort);
    }
    //else
    {
        // Over TCP
        session->setupStream(streamName, conn);
    }
    
    // Response
    RtspResponse response;
    response.setStatus(200);
    response.setHeader("CSeq",request->header("CSeq"));
    response.setHeader("Date","Thu, 15 Sep 2010 03:00:04 GMT");
    response.setHeader("Session", sid);
    
    // Stream name: rtx, audio, video
    if( streamName == "rtx" )
    {
        response.setHeader("RealChallenge3","d67b8f21bf272fd5020e9fbb08428cfa4f213d09,sdr=abcdabcd");
        
        std::ostringstream oss;
        oss << "RTP/AVP/UDP;unicast;server_port=" << serverPort << "-" << serverPort + 1
        << ";client_port=" << clientPort << "-" << clientPort + 1 << ";ssrc=f2bde83e;mode=PLAY";
        response.setHeader("Transport",oss.str());
    }
    else if(streamName == "audio" )
    {
        response.setHeader("RealChallenge3","d67b8f21bf272fd5020e9fbb08428cfa4f213d09,sdr=abcdabcd");
        response.setHeader("Transport","RTP/AVP/TCP;unicast;interleaved=2-3;ssrc=bedf8d08;mode=PLAY");
    }
    else if(streamName == "video" )
    {
        response.setHeader("Transport","RTP/AVP/TCP;unicast;interleaved=4-5;ssrc=bedf8d2d;mode=PLAY");
    }
    else
    {
        assert(false);
    }
    
    conn->SendResponse(&response);
}

// Get parameter of stream
void RtspServer::OnGetParamRequest(RtspRequest* request, RtspConnection* conn)
{
    assert(request != NULL);
    assert(conn != NULL);
    
    RtspResponse response;
    response.setStatus(200);
    response.setHeader("CSeq", request->header("CSeq"));
    response.setHeader("Session", request->header("Session"));
    
    conn->SendResponse(&response);
}

// Set parameter of stream
void RtspServer::OnSetParamRequest(RtspRequest* request, RtspConnection* conn)
{
    assert(request != NULL);
    assert(conn != NULL);
    
    RtspResponse response;
    std::string ping = request->header("Ping");
    response.setStatus(ping.empty() ? 200 : 451);
    response.setHeader("CSeq", request->header("CSeq"));
    response.setHeader("Session", request->header("Session"));
    
    conn->SendResponse(&response);
}

// Start to play
void RtspServer::OnPlayRequest(RtspRequest* request, RtspConnection* conn)
{
    assert(request != NULL);
    assert(conn != NULL);
    
    // Session
    std::string sid = request->header("Session");
    assert(!sid.empty());
    if(sid.empty())
    {
        return;
    }
    
    RtspSession* session = findSession(sid);
    assert(session != NULL);
    if(session == NULL)
    {
        return;
    }
    
    // Start position
    long nStartTime = -1;
    std::string strTime = request->header("Range");
    if( !strTime.empty() )
    {
        strTime = strTime.substr(4);
        size_t nPos = strTime.find("-");
        std::string strStartTime = strTime.substr(0, nPos);
        nStartTime = atoi(strStartTime.c_str());
    }
    
    // Seek to pos
    session->seek(nStartTime);
    
    // Response
    RtspResponse response;
    response.setStatus(200);
    response.setHeader("CSeq", request->header("CSeq"));
    response.setHeader("Session", request->header("Session"));
    std::string rtpInfo = session->streamsInfo();
    response.setHeader("RTP-Info",rtpInfo);
    
    conn->SendResponse(&response);
    
    // Play
    session->play();
}

void RtspServer::OnPauseRequest(RtspRequest* request, RtspConnection* conn)
{
    assert(request != NULL);
    assert(conn != NULL);
    
    // Session
    std::string sid = request->header("Session");
    assert(!sid.empty());
    if(sid.empty())
    {
        return;
    }
    
    RtspSession* session = findSession(sid);
    assert(session != NULL);
    if(session == NULL)
    {
        return;
    }

    // Pause
    session->pause();
    
    // Response
    RtspResponse response;
    response.setStatus(200);
    response.setHeader("CSeq", request->header("CSeq"));
    response.setHeader("Session",request->header("Session"));
    
    conn->SendResponse(&response);
}

// Close session
void RtspServer::OnTeardownRequest(RtspRequest* request, RtspConnection* conn)
{
    assert(request != NULL);
    assert(conn != NULL);
    
    // Session
    std::string sid = request->header("Session");
    assert(!sid.empty());
    if(sid.empty())
    {
        return;
    }
    
    RtspSession* session = findSession(sid);
    assert(session != NULL);
    if(session == NULL)
    {
        return;
    }
    
    // Response
    RtspResponse response;
    response.setStatus(200);
    response.setHeader("CSeq", request->header("CSeq"));
    response.setHeader("Session",request->header("Session"));
    
    conn->SendResponse(&response);
    
    // Teardown
    session->teardown();
}
