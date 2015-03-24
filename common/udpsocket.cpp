//
// udpsocket.cpp
//

#include "udpsocket.h"

UdpSocket::UdpSocket()
{
}

UdpSocket::~UdpSocket()
{
}

bool UdpSocket::DoBind(unsigned short port)
{
    assert(m_hSocket == INVALID_SOCKET );
    
    m_hSocket =	socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(m_hSocket == INVALID_SOCKET)	
    { 
        return false;
    }

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if(::bind(m_hSocket, (SOCKADDR*)&addr, sizeof(addr) ) == SOCKET_ERROR)	
	{
		printf("\n UdpSocket bind error : %d\n", OSGetLastError());
        OSCloseSocket(m_hSocket);
		m_hSocket = INVALID_SOCKET;
		return false;
	}

    return true;
}

void UdpSocket::SetPeer(const std::string& host, unsigned short port)
{
    memset(&m_peer, 0, sizeof(m_peer));
    m_peer.sin_family = AF_INET;
    m_peer.sin_port = htons(port);
    m_peer.sin_addr.s_addr = inet_addr(host.c_str());
}

ssize_t UdpSocket::Send(unsigned char* buf, size_t len)
{
    assert(m_hSocket != INVALID_SOCKET);
    return ::sendto(m_hSocket, (const char*)buf, len, 0, (sockaddr*)&m_peer, sizeof(m_peer));
}
