//
// Channelurl.cpp
//

#include "channelurl.h"

ChannelUrl::ChannelUrl(const std::string& url)
: _url(url)
{
    parse();
}

ChannelUrl::~ChannelUrl()
{
    
}

// protocol://2.3.4.5:8090/movies/test.wmv?source=...
void ChannelUrl::parse()
{
    size_t pos1 = _url.find("://");
    if(pos1 == std::string::npos)
    {
        return;
    }
    
    _protocol = _url.substr(0, pos1);
    
    size_t pos2 = _url.find("/", pos1 + 3);
    if(pos2 == std::string::npos)
    {
        return;
    }
    
    size_t pos3 = _url.find("?", pos1 + 3);
    if(pos3 == std::string::npos)
    {
        pos3 = _url.size();
    }
    
    assert(pos2 < pos3);
    _media = _url.substr(pos2 + 1, pos3 - pos2 - 1);
}

std::string ChannelUrl::url() const
{
    return _url;
}

std::string ChannelUrl::media() const
{
    return _media;
}