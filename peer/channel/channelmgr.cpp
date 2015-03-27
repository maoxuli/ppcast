//
// channelmgr.cpp
//

#include "channelmgr.h"

ChannelMgr theChannelMgr;

ChannelMgr::ChannelMgr()
{

}

ChannelMgr::~ChannelMgr()
{
    for (std::map<std::string, ChannelWrapper*>::iterator it = _channels.begin(); it != _channels.end(); ++it)
    {
        ChannelWrapper* wrapper = it->second;
        delete wrapper;
    }
}

bool ChannelMgr::StartChannel(const ChannelUrl& url)
{
    _locker.Lock();
    Channel* channel = NULL;
    std::map<std::string, ChannelWrapper*>::iterator it = _channels.find(url.media());
    if(it != _channels.end())
    {
        ChannelWrapper* wrapper = it->second;
        channel = wrapper->GetChannel();
        if(channel->stopped())
        {
            channel->Start();
            wrapper->AddRef();
        }
    }
    else
    {
        channel = new Channel(url);
        ChannelWrapper* wrapper = new ChannelWrapper(channel);
        _channels[channel->media()] = wrapper;
        channel->Start();
        wrapper->AddRef();
    }
    _locker.UnLock();
    return true;
}

void ChannelMgr::StopChannel(const std::string& media)
{
    _locker.Lock();
    std::map<std::string, ChannelWrapper*>::iterator it = _channels.find(media);
    if(it != _channels.end())
    {
        ChannelWrapper* wrapper = it->second;
        wrapper->GetChannel()->Stop();
        wrapper->Release();
        if(wrapper->GetChannel() == NULL)
        {
            _channels.erase(it);
            delete wrapper;
        }
    }
    _locker.UnLock();
}

std::vector<std::string> ChannelMgr::GetChannelList()
{
    std::vector<std::string> channels;
    _locker.Lock();
    for(std::map<std::string, ChannelWrapper*>::iterator it = _channels.begin(); it != _channels.end(); ++it)
    {
        ChannelWrapper* wrapper = it->second;
        channels.push_back(wrapper->GetChannel()->media());
    }
    _locker.UnLock();
    return channels;
}

Channel* ChannelMgr::GetChannel(const std::string& media)
{
    Channel* channel = NULL;
    
    _locker.Lock();
    std::map<std::string, ChannelWrapper*>::iterator it = _channels.find(media);
    if(it != _channels.end())
    {
        ChannelWrapper* wrapper = it->second;
        channel = wrapper->GetChannel();
        wrapper->AddRef();
    }
    _locker.UnLock();
    return channel;
}

void ChannelMgr::ReleaseChannel(const std::string& media)
{
    _locker.Lock();
    std::map<std::string, ChannelWrapper*>::iterator it = _channels.find(media);
    if(it != _channels.end())
    {
        ChannelWrapper* wrapper = it->second;
        wrapper->Release();
        if(wrapper->GetChannel() == NULL)
        {
            _channels.erase(it);
            delete wrapper;
        }
    }
    _locker.UnLock();
}
