//
// channelmgr.cpp
//

#include "channelmgr.h"

ChannelMgr::ChannelMgr()
{
    _rtspServer = NULL;
    _channel = NULL;
}

ChannelMgr::~ChannelMgr()
{
    if(_rtspServer != NULL)
    {
        delete _rtspServer;
    }
    
    if(_channel != NULL)
    {
        delete _channel;
    }
}

bool ChannelMgr::Initialize()
{
    // Start rtsp server
    assert(_rtspServer == NULL);
    _rtspServer = new RtspServer(this);
    if(!_rtspServer->Start())
    {
        delete _rtspServer;
        _rtspServer = NULL;
        return false;
    }
    
    // 
    return true;
}

void ChannelMgr::Shutdown()
{
    // 
    if(_rtspServer != NULL)
    {
        _rtspServer->Stop();
        delete _rtspServer;
        _rtspServer = NULL;
    }
    
    //
    if(_channel != NULL)
    {
        delete _channel;
        _channel = NULL;
    }
}

Channel* ChannelMgr::GetChannel(const ChannelUrl& url)
{
    return NULL;
}
