//
// channel.h
//

#ifndef	__CHANNEL_H__
#define	__CHANNEL_H__

#include "selector.h"
#include "channelurl.h"

class SourceClient;

class Channel : public Selector
{
public:
	Channel(const ChannelUrl& url);
	virtual ~Channel();
    
    bool Start();
    void Stop();
    
    std::string GetSDP();
    
    // Override from Selector
    virtual void OnRun();

protected:
    // Media
    ChannelUrl _url;
    
    std::string _sdp;
    
    // As a client of source server
    SourceClient* _sourceClient;
};

#endif 

