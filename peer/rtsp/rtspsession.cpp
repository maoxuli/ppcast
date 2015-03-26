//
// rtspsession.cpp
//

#include "rtspsession.h"
#include "rtspconnection.h"
#include "rtspstream.h"
#include "channelmgr.h"
#include "channel.h"
#include <sstream>

RtspSession::RtspSession(const std::string& sid, const std::string& cid)
: _sid(sid)
, _state(INIT)
{
    _channel = theChannelMgr.GetChannel(cid);
}

RtspSession::~RtspSession()
{
    if(_channel != NULL)
    {
        theChannelMgr.ReleaseChannel(_channel->id());
    }
}

bool RtspSession::run()
{
    return true;
}

void RtspSession::close()
{
    
}
    
std::string RtspSession::id()
{
    return _sid;
}

std::string RtspSession::media()
{
    return _media;
}

std::string RtspSession::streamsInfo()
{
    std::ostringstream oss;

    for(std::vector<RtspStream*>::iterator it = _streams.begin(); it != _streams.end(); ++it)
    {
        RtspStream* p = *it;
        if(p != NULL)
        {
            oss << "url=rtsp://127.0.0.1/" << _media << "/" << p->name() << ";seq=" << p->seq(false) << ";rtptime=0";
        }

        if(it != _streams.end())
        {
            oss << ",";
        }
    }

    return oss.str();
}

RtspStream* RtspSession::findStream(const std::string& name)
{
    for(std::vector<RtspStream*>::iterator it = _streams.begin(); it != _streams.end(); ++it)
    {
        RtspStream* p = *it;
        if(p != NULL && p->name() == name)
        {
            return p;
        }
    }

    return NULL;
}

void RtspSession::removeStream(const std::string& name)
{
    // Delete existing stream
    for(std::vector<RtspStream*>::iterator it = _streams.begin(); it != _streams.end(); )
    {
        RtspStream* p = *it;
        if(p != NULL && p->name() == name)
        {
            delete p;
            it = _streams.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

// RTSession must be inited sucessfully
// Based on port, util bind a local address successfully
bool RtspSession::setupStream(const std::string& name, unsigned short& serverPort, unsigned short clientPort) 
{
    // Remove existing stream
    removeStream(name);

    // Add a new stream
    RtpStream* pStream =new RtpStream(name);
    if(pStream != NULL && pStream->init(serverPort, clientPort))
    {
        _streams.push_back(pStream);
        _state = READY;

        return true;
    }

    return false;
}

bool RtspSession::setupStream(const std::string& name, RtspConnection* connection)
{
    // Remove existing stream
    removeStream(name);

    // Add a new stream
    TcpStream* pStream = new TcpStream(name);
    if(pStream != NULL && pStream->init(connection))
    {
        _streams.push_back(pStream);
        _state = READY;

        return true;
    }

    return false;
}

long RtspSession::seek(long pos)
{
    return 0;
}

void RtspSession::play()
{
    _state = PLAYING;
}

void RtspSession::pause()
{
    _state = READY;
}

void RtspSession::teardown()
{
    _state = INIT;
}
