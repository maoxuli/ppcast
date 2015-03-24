//
// TcpListener.cpp
//

#include "tcplistener.h"

TcpListener::TcpListener()
{

}

TcpListener::~TcpListener()
{

}

bool TcpListener::DoBind(const Endpoint& endpoint)
{
	if(m_hSocket != INVALID_SOCKET)	
    { 
        return false;
    }
    
	m_hSocket =	socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(m_hSocket == INVALID_SOCKET)	
    { 
        return false;
    }
    
    sockaddr_in addr = endpoint.GetSockAddr();
    int len = endpoint.GetSockAddrLen();
    
	if(::bind(m_hSocket, (SOCKADDR*)&addr, len) == SOCKET_ERROR)	
	{
		printf("\nTcpListener bind error : %d\n",OSGetLastError() );
        OSCloseSocket(m_hSocket);
		m_hSocket = INVALID_SOCKET;
		return false;
	}
    
	if(OSSetNonBlock(m_hSocket) == FALSE)
	{
		OSCloseSocket(m_hSocket);
		m_hSocket = INVALID_SOCKET;
		return false;
	}
    
	return true;
}

bool TcpListener::DoListen(int backlog)
{
	if(m_hSocket == INVALID_SOCKET )	
    { 
        return false;
    }
    
    if(listen(m_hSocket, backlog) != 0)
	{
		printf("\nTcpListener listen error : %d\n",OSGetLastError() );
		OSCloseSocket(m_hSocket);
		m_hSocket = INVALID_SOCKET;
		return false;
	}
    
	m_nState = kListening;
	return true;
}

void TcpListener::Close()
{
	if(m_hSocket != INVALID_SOCKET)
	{
		OSCloseSocket(m_hSocket);
		m_hSocket =	INVALID_SOCKET;
	}
    
	m_nState = kClosed;
}