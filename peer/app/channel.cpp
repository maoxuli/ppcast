//
// Channel.cpp
//

#include "channel.h"
#include "endpoint.h"
#include "sourceclient.h"

Channel::Channel(const ChannelUrl& url)
: _url(url)
{
    _sourceClient = NULL;
}

Channel::~Channel()
{
    Stop();
}

bool Channel::Start()
{
    if(StartThread())
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
    
    return true;
}

void Channel::Stop()
{
    StopThread();
}

void Channel::OnRun()
{
    while(m_bThread)
    {
        DoSelect();
    }
}

std::string Channel::GetSDP()
{
    return _sdp;
}