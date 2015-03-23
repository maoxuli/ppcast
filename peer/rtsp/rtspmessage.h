//
// rtspmessage.h
//

#ifndef __RTSP_MESSAGE_H__
#define __RTSP_MESSAGE_H__

#include "httpmessage.h"

typedef HttpMessage RtspMessage;

class RtspRequest : public HttpRequest
{
public:
    RtspRequest();
    RtspRequest(const std::string& version);
    virtual ~RtspRequest();
};

class RtspResponse : public HttpResponse

{
public:
    RtspResponse();
    RtspResponse(const std::string& version);
    virtual ~RtspResponse();
    
private:
    virtual std::string code2reason(int code);
};

#endif 

