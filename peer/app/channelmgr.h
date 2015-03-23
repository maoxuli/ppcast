//
// channelmgr.h
//

#ifndef	__CHANNEL_MGR_H__
#define	__CHANNEL_MGR_H__

#include "channel.h"
#include "rtspserver.h"
#include "channelurl.h"

// A peer server is a channel manager
// (currently) it runs a single channel at a time
// A mini rtsp server interface with peer client
class ChannelMgr  
{
public:
	ChannelMgr();
	virtual ~ChannelMgr();
	
    bool Initialize();
    void Shutdown();
    
    Channel* GetChannel(const ChannelUrl& url);
    
private:
    RtspServer* _rtspServer;
    Channel* _channel;
};

extern ChannelMgr theChannelMgr;

#endif
