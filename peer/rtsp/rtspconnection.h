//
// rtspconnection.h
//

#ifndef __RTSP_CONNECTION_H__
#define __RTSP_CONNECTION_H__

#include "tcpconnection.h"

// 
// RtspConnection is extension of TcpConnection
// Handle RTSP packet and forward to RtspServer
//

class RtspServer;

class RtspConnection : public TcpConnection
{
public:
    RtspConnection(); // For client 
    RtspConnection(pputil::SOCKET fd, RtspServer* server); // For server
    virtual ~RtspConnection();

    // For client, send request to server
    bool sendRequest(RtspRequest* msg);
    
    // For server, send response to client
    // send interleaved data to client
    bool sendResponse(RtspResponse* msg);
    bool sendData(byte* b, size_t n);
    
private:
    // From Connection
    virtual void onReceive();
        
    // Parse packet
    void readType();
    void readData();
    void readInitial();
    void readHeader();
    void readBody();
    void onMessage();
    
    // For client, receive data and response
    virtual onData(Buffer* buffer) = 0;
    virtual onResponse(RtspResponse* msg) = 0;

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
    Message* _message;  // RtspRequest or RtspResponse message
    size_t _bodyLen;	// Length of body
};

#endif 
