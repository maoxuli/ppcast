//
// Channel.cpp
//

#include "channel.h"
#include "endpoint.h"
#include "sourceclient.h"
#include "rtspsession.h"

Channel::Channel(const ChannelUrl& url)
: _url(url)
, _stopped(true)
{
    _sourceClient = NULL;
}

Channel::~Channel()
{
    Stop();
}

bool Channel::Start()
{
    if(!StartThread())
    {
        return false;
    }
    
    if(_sourceClient == NULL)
    {
        Endpoint endpoint;
        endpoint.Set("192.168.1.5", 5355);
        _sourceClient = new SourceClient(endpoint);
        
        if(_sourceClient->Initialize())
        {
            Add(_sourceClient);
        }
    }
    
    _stopped = false;
    return true;
}

void Channel::Stop()
{
    StopThread();
    _stopped = true;
}

bool Channel::stopped() const
{
    return _stopped;
}

void Channel::OnRun()
{
    while(m_bThread)
    {
        DoSelect();
    }
}

std::string Channel::media() const
{
    return _url.media();
}

std::string Channel::sdp() const
{
    return _sdp;
}