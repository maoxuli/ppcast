#include "network.h"

Socket::Socket()
{
	m_nState = kClosed;
	m_hSocket = INVALID_SOCKET;
}

Socket::~Socket()
{
	Close();
}

void Socket::Close()
{
	if ( m_hSocket != INVALID_SOCKET )
	{
		OSCloseSocket( m_hSocket );
		m_hSocket	=	INVALID_SOCKET;
	}
	m_nState = kClosed;
}

void Socket::SetState(int state)
{
    m_nState = state;
}

bool Socket::OnRun()
{
    return true;
}

bool Socket::OnAccept()
{
	return true;
}

bool Socket::OnConnected()
{
    return true;
}

void Socket::OnDropped(int nError)
{
	return;
}

bool Socket::OnRead()
{
    return true;
}

bool Socket::OnWrite()
{
    return true;
}



