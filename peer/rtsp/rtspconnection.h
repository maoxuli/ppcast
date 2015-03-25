//
// rtspconnection.h
//

#ifndef __RTSP_CONNECTION_H__
#define __RTSP_CONNECTION_H__

#include "tcpconnection.h"
#include "rtspmessage.h"

class RtspServer;

// RtspConnection is extension of TcpConnection
// Handle RTSP packet and forward to RtspServer

class RtspConnection : public TcpConnection
{
public:
    RtspConnection(RtspServer* server); 
    virtual ~RtspConnection();

    // For server, send response to client
    // send interleaved data to client
    bool SendResponse(RtspResponse* msg);
    bool SendData(unsigned char* buf, size_t len);
    
private:
    // From Connection
    virtual bool OnRead();
        
    // Parse packet
    void ReadType();
    void ReadData();
    void ReadInitial();
    void ReadHeader();
    void ReadBody();
    void OnMessage();
    
private:
    // Owner RTSP server
    RtspServer* _server;

    // State Machine to receive RTSP message
    enum RTSP_MESSAGE_STATE
    {
        RMS_READY,        // Ready to receive a new packet
        RMS_READ_DATA,    // Reading interleaved raw data packet
        RMS_READ_INITIAL, // Reading initial line of request or response
        RMS_READ_HEADER,  // Reading header lines
        RMS_READ_BODY,    // Reading body
        RMS_READ_OK       // Complete a message packet
    };

    RTSP_MESSAGE_STATE _state;

    // Incoming Message packet
    RtspMessage* _message;  // RtspRequest or RtspResponse message
    size_t _bodyLen;	// Length of body
};

#endif 
