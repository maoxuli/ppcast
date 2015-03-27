//
// channelurl.h
//

#ifndef	__CHANNEL_URL_H__
#define	__CHANNEL_URL_H__

#include "os.h"

class ChannelUrl
{
public:
    ChannelUrl(const std::string& url);
	virtual ~ChannelUrl();
    
    std::string url() const;
    std::string media() const;
    
private:
    std::string _url;
    
    std::string _protocol;
    std::string _media;
    
    void parse();
};

#endif 

