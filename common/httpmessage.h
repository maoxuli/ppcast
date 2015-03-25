//
// httpmessage.h
//

#ifndef __HTTP_MESSAGE_H__
#define __HTTP_MESSAGE_H__

#include "os.h"

class Buffer;

class HttpMessage
{
public:
    class MessageHeader
    {
    private:
        MessageHeader();
        
    public:
        MessageHeader(const std::string& key);
        MessageHeader(const std::string& key, const std::string& value);
        
        std::string  key() const;
        std::string  value() const;
        void  setValue(const std::string& value);
        
    protected:
        std::string _key;
        std::string _value;
    };
    
    typedef std::vector<MessageHeader*> MessageHeaderSeq;
    enum MESSAGE_TYPE {UNKNOWN_MESSAGE, REQUEST_MESSAGE, RESPONSE_MESSAGE};

private:
    bool operator==(const HttpMessage& msg) const;
    bool operator!=(const HttpMessage& msg) const;
    const HttpMessage& operator=(const HttpMessage& msg); 

public:
    HttpMessage();
    HttpMessage(const std::string& version);
    virtual ~HttpMessage();

    virtual MESSAGE_TYPE type() const;
    virtual std::string dump() const = 0;
    virtual bool toBuffer(Buffer* buffer) const = 0;

    std::string version() const;
    void setVersion(const std::string& version);

    std::string header(const std::string& key) const;
    void setHeader(const std::string& key, const std::string& value);
    void setHeader(const MessageHeader& header);
    void removeHeader(const std::string& key);

    // Total header length for key/val pairs (incl. ": " and CRLF)
    // but NOT separator CRLF
    size_t headerLen() const;
    size_t headerCount() const;
    MessageHeader* header(size_t index) const;

    // Body section
    size_t bodyLen() const;
    char* body() const;
    void setBody(const char* buf, size_t len);

protected:
    std::string		_protocol;		// "HTTP", "RTSP", ...
    std::string		_version;       // "1.0", "1.1", "2.0"
    MessageHeaderSeq _headers;
    size_t			_bodyLen;
    char*           _body;
};

class HttpRequest : public HttpMessage
{
public:
    HttpRequest();
    HttpRequest(const std::string& version);
    virtual ~HttpRequest();

    virtual MESSAGE_TYPE type() const;
    virtual std::string dump() const;
    virtual bool toBuffer(Buffer* buffer) const;

    std::string	method() const;
    void setMethod(const std::string& method);

    std::string url() const;
    void setUrl(const std::string& url);

protected:
    std::string	_method;
    std::string _url;
};

class HttpResponse : public HttpMessage
{
public:
    HttpResponse();
    HttpResponse(const std::string& version);
    virtual ~HttpResponse();

    virtual MESSAGE_TYPE type() const;
    virtual std::string dump() const;
    virtual bool toBuffer(Buffer* buffer) const;

    int code() const;
    std::string reason() const;
    void setStatus(int code, const std::string& reason = "");

protected:
    int _code;
    std::string _reason;
    
    virtual std::string code2reason(int code);
};

#endif 

