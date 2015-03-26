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
    std::map<std::string, ChannelWrapper*>::iterator it = _channels.find(url.cid());
    if(it != _channels.end())
    {
        ChannelWrapper* wrapper = it->second;
        channel = wrapper->GetChannel();
    }
    else
    {
        channel = new Channel(url);
        ChannelWrapper* wrapper = new ChannelWrapper(channel);
        _channels[channel->id()] = wrapper;
    }
    channel->Start();
    _locker.UnLock();
    return true;
}

void ChannelMgr::StopChannel(const std::string& cid)
{
    _locker.Lock();
    std::map<std::string, ChannelWrapper*>::iterator it = _channels.find(cid);
    if(it != _channels.end())
    {
        ChannelWrapper* wrapper = it->second;
        wrapper->GetChannel()->Stop();
    }
    _locker.UnLock();

}

Channel* ChannelMgr::GetChannel(const std::string& cid)
{
    Channel* channel = NULL;
    
    _locker.Lock();
    std::map<std::string, ChannelWrapper*>::iterator it = _channels.find(cid);
    if(it != _channels.end())
    {
        ChannelWrapper* wrapper = it->second;
        channel = wrapper->GetChannel();
        wrapper->AddRef();
    }
    _locker.UnLock();
    return channel;
}

void ChannelMgr::ReleaseChannel(const std::string& cid)
{
    _locker.Lock();
    std::map<std::string, ChannelWrapper*>::iterator it = _channels.find(cid);
    if(it != _channels.end())
    {
        ChannelWrapper* wrapper = it->second;
        wrapper->Release();
    }
    _locker.UnLock();
}
