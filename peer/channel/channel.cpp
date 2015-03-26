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
    Close();
}

bool Channel::Intialize()
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

void Channel::Close()
{
    StopThread();
    
    // Notify all rtsp session
    for(std::vector<RtspSession*>::iterator it = _sessions.begin(); it != _sessions.end(); ++it)
    {
        RtspSession* session = *it;
        assert(session != NULL);
        session->setChannel(this);
    }
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