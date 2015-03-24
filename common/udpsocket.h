//
// udpsocket.h
//

#ifndef __UDP_SOCKET_H__
#define __UDP_SOCKET_H__

#include "network.h"

class UdpSocket : public Socket
{
public:
    UdpSocket();
    virtual ~UdpSocket();

    // Init with local port number
    bool DoBind(unsigned short port);

    // Peer address
    void SetPeer(const std::string& host, unsigned short port);
    ssize_t Send(unsigned char* b, size_t n);

public:
    sockaddr_in m_peer;
};

#endif 
