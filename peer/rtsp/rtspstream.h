//
// rtspstream.h
//

#ifndef __RTSP_STREAM_H__
#define __RTSP_STREAM_H__

#include "udpsocket.h"

class RtspConnection;

// RTSP server using a session to keep state of a presentation
// A RTSPSession using one or more streams to send media to client
// The stream may be RTP based or TCP based

class RtspStream
{
public:
    RtspStream(const std::string& name);
    virtual ~RtspStream();

    // Configuration name of the stream
    // SDP and SETUP of RTSP request
    std::string name();

    // update == true, get current seq and generate new one
    // update == false, peek current seq only
    unsigned int seq(bool update = true);

    // Data send interface
    virtual bool sendData(unsigned char* buf, size_t len) = 0;

protected:
    std::string _name;
    unsigned int _seq;
};

class RtpStream : public RtspStream
{
public:
    RtpStream(const std::string& name);
    virtual ~RtpStream();

    bool init(unsigned short& serverPort, unsigned short clientPort);
    virtual bool sendData(unsigned char* buf, size_t len);

private:
    UdpSocket _rtpSocket;
    UdpSocket _rtcpSocket;
};


class TcpStream : public RtspStream
{
public:
    TcpStream(const std::string& name);
    virtual ~TcpStream();

    bool init(RtspConnection* conn);
    virtual bool sendData(unsigned char* b, size_t len);

private:
    RtspConnection* _connection;
};

#endif 
