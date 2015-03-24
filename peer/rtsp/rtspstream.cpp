//
// rtspstream.cpp
//

#include "rtspstream.h"

RtspStream::RtspStream(const std::string& name)
: _name(name)
, _seq(0)
{

}

RtspStream::~RtspStream()
{

}

std::string RtspStream::name()
{
    return _name;
}

unsigned int RtspStream::seq(bool update)
{ 
    return update ? _seq++ : _seq;
}

/////////////////////////////////////////////////////////////////////

RtpStream::RtpStream(const std::string& name)
: RtspStream(name)
{

}

RtpStream::~RtpStream()
{
    _rtpSocket.Close();
    _rtcpSocket.Close();
}

bool RtpStream::init(unsigned short& serverPort, unsigned short clientPort)
{
    // Try to do until available ports
    for(int i=0; i<10; i++)
    {
        if(_rtpSocket.DoBind(serverPort))
        {
            if(_rtcpSocket.DoBind(serverPort + 1))
            {
                break;
            }
            else
            {
                _rtpSocket.Close();
            }
        }

        serverPort += 2;
    }

    _rtpSocket.SetPeer("127.0.0.1", clientPort);
    _rtcpSocket.SetPeer("127.0.0.1", clientPort + 1);

    return true;
}

bool RtpStream::sendData(unsigned char* buf, size_t len)
{
    ssize_t ret = _rtpSocket.Send(buf, len);
    return ret == len;
}

//////////////////////////////////////////////////////////////////////

TcpStream::TcpStream(const std::string& name)
: RtspStream(name)
, _connection(NULL)
{

}

TcpStream::~TcpStream()
{
    _connection = NULL;
}

bool TcpStream::init(RtspConnection* conn)
{
    _connection = conn;
    return true;
}

bool TcpStream::sendData(unsigned char* buf, size_t len)
{
    if(_connection != NULL)
    {
        _connection->SendData(buf, len);
        return true;
    }

    return false;
}
