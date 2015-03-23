//
// httpmessage.cpp
//

#include "httpmessage.h"
#include "buffer.h"
#include <sstream>

HttpMessage::MessageHeader::MessageHeader(const std::string& key)
: _key(key)
{

}

HttpMessage::MessageHeader::MessageHeader(const std::string& key, const std::string& value)
: _key(key)
, _value(value)
{

}

std::string HttpMessage::MessageHeader::key() const
{
    return _key;
}

std::string HttpMessage::MessageHeader::value() const
{
    return _value;
}

void HttpMessage::MessageHeader::setValue(const std::string& value)
{
   _value = value;
}

/////////////////////////////////////////////////////////////////////////

HttpMessage::HttpMessage() 
{
    _protocol = "HTTP";
    _version = "1.0";
    _bodyLen = 0;
    _body = NULL;
}

HttpMessage::HttpMessage(const std::string& version)
{
    _protocol = "HTTP";
    _version = version;
    _bodyLen = 0;
    _body = NULL;
}

HttpMessage::~HttpMessage()
{
    for(MessageHeaderSeq::iterator it = _headers.begin(); it != _headers.end(); ++it)
    {
        MessageHeader* pHeader = *it;
        if(pHeader != NULL)
        {
            delete pHeader;
        }
    }
    _headers.clear();

    if(_body != NULL)
    {
        delete[] _body; 
        _body = NULL;
        _bodyLen = 0;
    }
}

HttpMessage::MESSAGE_TYPE HttpMessage::type() const
{
    return UNKNOWN_MESSAGE;
}

std::string HttpMessage::version() const
{
    return _version;
}

void HttpMessage::setVersion(const std::string& version)
{
    _version = version;
}

std::string HttpMessage::header(const std::string& key) const
{
    std::string value;
    MessageHeaderSeq::const_iterator it(_headers.begin());
    while(it != _headers.end())
    {
        MessageHeader* pHeader = *it;
        if(key == pHeader->key())
        {
            value = pHeader->value();
            break;
        }
        ++it;
    }

    return value;
}

void HttpMessage::setHeader(const std::string& key, const std::string& value)
{
    MessageHeaderSeq::iterator it(_headers.begin());
    while(it != _headers.end())
    {
        MessageHeader* pHeader = *it;
        if(key == pHeader->key())
        {
            pHeader->setValue(value);
            return;
        }
        ++it;
    }

    _headers.push_back(new MessageHeader(key, value));
}

void HttpMessage::setHeader(const MessageHeader& header)
{
    MessageHeaderSeq::iterator it(_headers.begin());
    while(it != _headers.end())
    {
        MessageHeader* pHeader = *it;
        if(header.key() == pHeader->key())
        {
            pHeader->setValue(header.value());
            return;
        }
        ++it;
    }

    _headers.push_back(new MessageHeader(header));
}

void HttpMessage::removeHeader(const std::string&key) 
{
    MessageHeaderSeq::iterator it(_headers.begin());
    while(it != _headers.end())
    {
        MessageHeader* pHeader = *it;
        if(key == pHeader->key())
        {
            it = _headers.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

size_t HttpMessage::headerLen() const
{
    size_t nLen = 0;
    MessageHeaderSeq::const_iterator it(_headers.begin());
    while(it != _headers.end())
    {
        MessageHeader* pHeader = *it;
        if(pHeader != NULL)
        {
            nLen += (pHeader->key().size() + 2 + pHeader->value().size() + 2);
        }
        ++it;
    }

    return nLen;
}

size_t HttpMessage::headerCount() const
{
    return _headers.size();
}

HttpMessage::MessageHeader* HttpMessage::header(size_t index) const
{
    if(index >= _headers.size())
    {
        return NULL;
    }

    return _headers.at(index);
}

size_t HttpMessage::bodyLen() const
{
    return _bodyLen;
}

char* HttpMessage::body() const
{
    return _body;
}

void HttpMessage::setBody(char* buf, size_t len)
{
    if(_body != NULL)
    {
        delete[] _body; 
        _body = NULL;
    }

    _bodyLen = len;
    if(_bodyLen > 0)
    {
        _body = new char[_bodyLen];
        memcpy(_body, buf, _bodyLen);
    }
}

//////////////////////////////////////////////////////////////////////////

HttpRequest::HttpRequest() 
: HttpMessage()
{
    _method = "GET";
}

HttpRequest::HttpRequest(const std::string& version) 
: HttpMessage(version)
{
    _method = "GET";
}

HttpRequest::~HttpRequest()
{

}

HttpMessage::MESSAGE_TYPE HttpRequest::type() const
{
    return HttpMessage::REQUEST_MESSAGE;
}

std::string HttpRequest::dump() const
{
    // <verb> SP <url> SP <protocol/version> CRLF
    // <headers> CRLF 
    // <buf>
    std::ostringstream oss;

    oss << _method << " " << _url << " " << _protocol << "/" << _version << "\r\n";

    for(size_t i = 0; i < headerCount(); i++)
    {
        MessageHeader* pHeader = header(i);
        oss << pHeader->key() << ":" << pHeader->value() << "\r\n";
    }

    oss << "\r\n";

    oss << "Body (" << _bodyLen << "bytes)\r\n";

    return oss.str();
}

bool HttpRequest::toBuffer(Buffer* buffer) const
{
    return true;
}

std::string HttpRequest::method() const
{
    return _method;
}

void HttpRequest::setMethod(const std::string& method)
{
    _method =  method;
}

std::string HttpRequest::url() const
{
    return _url;
}

void HttpRequest::setUrl(const std::string& url)
{
    _url = url;
}

//////////////////////////////////////////////////////////////////////////////////

HttpResponse::HttpResponse() 
: HttpMessage() 
{
    _code = 0;
}

HttpResponse::HttpResponse(const std::string& version) 
: HttpMessage(version)
{
    _code = 0;
}

HttpResponse::~HttpResponse()
{

}

HttpMessage::MESSAGE_TYPE HttpResponse::type() const
{
    return HttpMessage::RESPONSE_MESSAGE;
}

std::string HttpResponse::dump() const
{
    // <protocol>/<version> code message CRLF
    // <headers> CRLF 
    // <buf>
    std::ostringstream oss;

    oss << _protocol << "/" << _version << " " << _code << " " << _reason << "\r\n";

    for(size_t i = 0; i < headerCount(); i++)
    {
        MessageHeader* pHeader = header(i);
        oss << pHeader->key() << ":" << pHeader->value() << "\r\n";
    }

    oss << "\r\n";

    oss << "Body (" << _bodyLen << "bytes)" << "\r\n";

    return oss.str();
}

bool HttpResponse::toBuffer(Buffer* buffer) const
{
    /*
    unsigned int nCode = msg->code();
    std::string sReason = msg->message();
    size_t nHdrLen = msg->headerLen();
    size_t nBufLen = msg->bufferLen();
    
    // "RTSP/1.0" SP <code> SP <reason> CRLF
    // <headers> CRLF
    // <buf> (or terminating NULL from sprintf() if no buffer)
    size_t size = 8 + 1 + 3 + 1 + sReason.length() + 2 + nHdrLen + 2;
    size += nBufLen != 0 ? nBufLen : 1;
    
    char* buf = new char[size];
    assert(buf != NULL);
    if(buf == NULL)
    {
        std::cout << "RTSPServer send response out of memory.\n";
        return false;
    }
    
    char* p = buf;
    p += sprintf(p, "RTSP/1.0 %u %s\r\n", nCode, sReason.c_str());
    
    for(UINT n = 0; n < msg->headerCount(); n++ )
    {
        RTSPHeader* pHeader = msg->header(n);
        p += sprintf(p, "%s: %s\r\n", pHeader->key().c_str(), pHeader->value().c_str());
    }
    p += sprintf(p, "\r\n");
    
    if( nBufLen )
    {
        memcpy(p, msg->buffer(), nBufLen );
        p += nBufLen;
    }
    
    m_pOutput->Add(buf, p - buf);
    OnWrite();
    delete[] buf;
*/
    return true;
}

int HttpResponse::code() const
{
    return _code;
}

std::string HttpResponse::reason() const
{
    return _reason;
}

void HttpResponse::setStatus(int code, const std::string& reason)
{
    _code = code;
    _reason = reason;

    if(_reason.empty())
    {
        _reason = code2reason(_code);
    }
}

// Rtsp response status code and reason map
struct HttpResponseStatus 
{ 
    int code; 
    const char* reason; 
};

// These must be sorted
static HttpResponseStatus s_pHttpStatus[] =
{
    { 100, "Continue" },
    
    { 200, "OK" },
    { 201, "Created" },
    { 250, "Low on Storage Space" },
    
    { 300, "Multiple Choices" },
    { 301, "Moved Permanently" },
    { 302, "Moved Temporarily" },
    { 303, "See Other" },
    { 304, "Not Modified" },
    { 305, "Use Proxy" },
    
    { 400, "Bad Request" },
    { 401, "Unauthorized" },
    { 402, "Payment Required" },
    { 403, "Forbidden" },
    { 404, "Not Found" },
    { 405, "Method Not Allowed" },
    { 406, "Not Acceptable" },
    { 407, "Proxy Authentication Required" },
    { 408, "Request Time-out" },
    { 410, "Gone" },
    { 411, "Length Required" },
    { 412, "Precondition Failed" },
    { 413, "Request Entity Too Large" },
    { 414, "Request-URI Too Large" },
    { 415, "Unsupported Media Type" },
    { 451, "Parameter Not Understood" },
    { 452, "Conference Not Found" },
    { 453, "Not Enough Bandwidth" },
    { 454, "Session Not Found" },
    { 455, "Method Not Valid in This State" },
    { 456, "Header Field Not Valid for Resource" },
    { 457, "Invalid Range" },
    { 458, "Parameter Is Read-Only" },
    { 459, "Aggregate operation not allowed" },
    { 460, "Only aggregate operation allowed" },
    { 461, "Unsupported transport" },
    { 462, "Destination unreachable" },
    
    { 500, "Internal Server Error" },
    { 501, "Not Implemented" },
    { 502, "Bad Gateway" },
    { 503, "Service Unavailable" },
    { 504, "Gateway Time-out" },
    { 505, "Rtsp Version not supported" },
    { 551, "Option not supported" },
    {   0, NULL }
};

static const size_t s_nHttpStatus = sizeof(s_pHttpStatus)/sizeof(HttpResponseStatus) - 1;

std::string HttpResponse::code2reason(int code)
{
    int hi = s_nHttpStatus;
    int lo = -1;
    int mid;
    while(hi - lo > 1)
    {
        mid = (hi + lo)/2;
        if(_code <= s_pHttpStatus[mid].code)
            hi = mid;
        else
            lo = mid;
    }
    
    if(_code == s_pHttpStatus[hi].code)
    {
        return s_pHttpStatus[hi].reason;
    }
    
    return "Unknown";
}
