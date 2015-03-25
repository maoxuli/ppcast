//
// rtspconnection.cpp
//

#include "rtspconnection.h"
#include "rtspserver.h"
#include "buffer.h"
#include <sstream>

RtspConnection::RtspConnection(RtspServer* server)
: _server(server)
, _state(RMS_READY)
, _message(NULL)
, _bodyLen(0)
{
    assert(_server != NULL);
}

RtspConnection::~RtspConnection()
{
    if(_message != NULL)
    {
        delete _message;
        _message = NULL;
    }
}

// Send interleaved data
bool RtspConnection::SendData(unsigned char* buf, size_t len)
{
    m_pInput->write8u('$');
    m_pInput->write8u('$');
    m_pInput->write16u(len);
    m_pInput->write(buf, len);
    
    OnWrite();
    return true;
}

// Send a response message
bool RtspConnection::SendResponse(RtspResponse* msg)
{
    assert(msg != NULL);

//#ifdef _DEBUG
    std::cout << ">>> " << msg->dump();
//#endif
    
    msg->toBuffer(m_pOutput);
    
    OnWrite();
    return true;
}

// Called when data is received and appended to _inBuffer
// Parse RtspRequest, forward to RtspServer to handle
bool RtspConnection::OnRead()
{
    if(!TcpConnection::OnRead())
    {
        return false;
    }
    
    std::cout << "RtspConnection:: OnRead()\n";
    bool ret = true;
    while (ret) 
    {
        switch(_state) 
        {
            case RMS_READY:
                ret = ReadType();
                break;
            case RMS_READ_DATA:
                ret = ReadData();
                break;
            case RMS_READ_INITIAL:
                ret = ReadInitial();
                break;
            case RMS_READ_HEADER:
                ret = ReadHeader();
                break;
            case RMS_READ_BODY:
                ret = ReadBody();
                break;
            default:
                ret = false;
                break;
        }
    }
    
    if(_state == RMS_READ_OK)
    {
        OnMessage();
    }
    
    return true;
}
  
// First two bytes marks the type of data
// $$ leads to a raw data packet (2 bytes dataLen + data)
// otherwise RTSP message packet
bool RtspConnection::ReadType()
{
    std::cout << "read type\n";
    assert(_state == RMS_READY);
    assert(_message == NULL);
    assert(_bodyLen == 0);
    
    if(m_pInput->readable() < 1)
    {
        return false;
    }
    
    _state = (m_pInput->peek8u() == '$') ? RMS_READ_DATA : RMS_READ_INITIAL;
    return true;
}

// Read raw data packet  
bool RtspConnection::ReadData()
{
    std::cout << "read data\n";
    assert(_state == RMS_READ_DATA);
    assert(m_pInput->readable() >= 1);
    assert(m_pInput->peek8u() == '$');
    assert(_message == NULL);
    assert(_bodyLen == 0);
   
    // Check data length and availability in buffer
    if(m_pInput->readable() < 4)
    {
       return false;
    }

    size_t dataLen = m_pInput->peek16u(2);
    assert(dataLen > 0);

    if(m_pInput->readable() < dataLen + 4)
    {
        return false;
    }
    
    // Read data
    m_pInput->remove(4);

    Buffer dataBuffer;
    dataBuffer.write(m_pInput->peek(), dataLen);
    m_pInput->remove(dataLen);
    //OnData(&dataBuffer);
   
    _state = RMS_READY;
    return true;
}

// Read initial line of RTSP message
// Request: <verb> <url> RTSP/1.0
// Response: RTSP/1.0 <code> <reason>
bool RtspConnection::ReadInitial()
{
    std::cout << "read initial\n";
    assert(_state == RMS_READ_INITIAL);
    assert(_message == NULL);
    assert(_bodyLen == 0);

    // Read a line
    std::string initialLine;
    m_pInput->readString(initialLine, "\r\n");
    if(initialLine.empty())
    {
        return false;
    }

    // Split string with space
    std::istringstream iss(initialLine);
    std::string sVerb;
    iss >> sVerb;

    if(sVerb.substr(0,4) == "RTSP")
    {
        // Response: RTSP/#.# <code> <reason>
        std::string sCode;
        iss >> sCode;

        RtspResponse* pmsg = new RtspResponse();
        pmsg->setStatus(atoi(sCode.c_str()));
        _message = pmsg;
    }
    else
    {
        // Request: <verb> <url> RTSP/#.#
        std::string sUrl;
        iss >> sUrl;

        RtspRequest* pmsg = new RtspRequest();
        pmsg->setMethod(sVerb);
        pmsg->setUrl( sUrl );
        _message = pmsg;
    }
    
    _state = RMS_READ_HEADER;
    std::cout << "initial line: " << initialLine << "\n";
    std::cout << "size: " << m_pInput->readable() << "\n";
    return true;
}

// Read header lines
// Read each complete line, until a blank line
bool RtspConnection::ReadHeader()
{
    std::cout << "read header\n";
    assert(_state == RMS_READ_HEADER);
    assert(_message != NULL);
    assert(_bodyLen == 0);

    // Read a lines each time
    std::string line;
    
    if(!m_pInput->readString(line, "\r\n"))
    {
        return false;
    }
    
    std::cout << line << "\n";
    if(line.empty())
    {
        // Separator line of headers and body
        std::string sBodyLen = _message->header("Content-Length");
        if(!sBodyLen.empty())
        {
            _bodyLen = atoi(sBodyLen.c_str());
        }
        _state = _bodyLen > 0 ? RMS_READ_BODY : RMS_READ_OK;
        return true;
    }
    
    if(line.size() > 512)
    {
        // Line is too long
        std::cout << "RtspConnection::readHeader() too long line.\n";
        return false;
    }

    // Key and value
    size_t pos = line.find(":");
    assert(pos != std::string::npos);
    std::string key = line.substr(0, pos);
    std::string value = line.substr(pos + 1);
    _message->setHeader(key, value);

    std::cout << "head line: " << line << "\n";
    std::cout << "size: " << m_pInput->readable() << "\n";

    return true;
}

bool RtspConnection::ReadBody()
{
    std::cout << "read body\n";
    assert(_state == RMS_READ_BODY);
    assert(_message != NULL);
    assert(_bodyLen > 0);
    
    if(m_pInput->readable() < _bodyLen)
    {
        return false;
    }
    
    _message->setBody((char*)m_pInput->peek(), _bodyLen);
    m_pInput->remove(_bodyLen);
    _state = RMS_READ_OK;
    return true;
}

// Called when a RTSP message is parsed
// Forward to RtspServer
void RtspConnection::OnMessage()
{
    std::cout << "On message\n";
    assert(_state = RMS_READ_OK);
    assert(_message != NULL);
    
#ifdef _DEBUG
    std::cout << "<<< " << _message->dump(); 
#endif
    
    switch(_message->type())
    {
        case RtspMessage::REQUEST_MESSAGE:
        {
            assert(_server != NULL);
            RtspRequest* pmsg = dynamic_cast<RtspRequest*>(_message);
            _server->OnRequest(pmsg, this);
            break;
        }
        case RtspMessage::RESPONSE_MESSAGE:
        {
            RtspResponse* pmsg = dynamic_cast<RtspResponse*>(_message);
            //OnResponse(pmsg);
            break;
        }
        default:
            assert(false);
            break;
    }

    delete _message; 
    _message = NULL;
    _bodyLen = 0;
    _state = RMS_READY;
}
