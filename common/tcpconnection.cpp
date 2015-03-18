//
// TcpConnection.cpp
//

#include "TcpConnection.h"
#include "Endpoint.h"
#include "Buffer.h"
#include "os.h"

TcpConnection::TcpConnection()
{

	m_pInput = NULL;
	m_pOutput = NULL;
}

TcpConnection::~TcpConnection(void)
{

}

bool TcpConnection::DoBind(Endpoint ep)
{
	if(m_hSocket != INVALID_SOCKET )	
    { 
        return false;
    }

	m_hSocket =	socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(m_hSocket == INVALID_SOCKET)	
    { 
        return false;
    }

	if(bind(m_hSocket, (SOCKADDR*)&ep.m_sockaddr, ep.m_socklen ) == SOCKET_ERROR)	
	{
		printf("\n TcpConnection bind error : %d\n", OSGetLastError());
		m_hSocket = INVALID_SOCKET;
		return false;
	}

	if(OSSetNonBlock(m_hSocket) == false)
	{
		OSCloseSocket(m_hSocket);
		m_hSocket = INVALID_SOCKET;
		return false;
	}

	m_LocalEndpoint	= ep;

	return true;
}

bool TcpConnection::DoConnect(const Endpoint& endpoint)
{
	if(m_hSocket != INVALID_SOCKET )
    { 
        return false;
    }

	m_hSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(m_hSocket == INVALID_SOCKET)	
    { 
        return false;
    }

	if(OSSetNonBlock(m_hSocket ) == false)
	{
		OSCloseSocket(m_hSocket);
		m_hSocket = INVALID_SOCKET;
		return false;
	}

    sockaddr_in addr = endpoint.GetSockAddr();
    int len = endpoint.GetSockAddrLen();
	if(::connect(m_hSocket, (SOCKADDR*)&addr, len ) == SOCKET_ERROR)
	{
		UInt32 nError = OSGetLastError();

		if(nError != EWOULDBLOCK && nError != EINPROGRESS)
		{
			printf("\nTcpConnection connect error %d\n", nError);
			OSCloseSocket( m_hSocket );
			m_hSocket = INVALID_SOCKET;
			return false;
		}
	}

	if(m_pInput != NULL)	
    { 
        delete m_pInput;
    }
	if(m_pOutput != NULL)	
    { 
        delete m_pOutput;
    }
    
    m_pInput = new Buffer();
    m_pOutput = new Buffer();

	m_nState = kConnecting;
	m_RemoteEndpoint = endpoint;
	
	return true;
}

bool TcpConnection::AttachTo(SOCKET hSocket)
{
	if(m_hSocket != INVALID_SOCKET )	
    { 
        return false;
    }

	m_hSocket = hSocket;
	m_RemoteEndpoint.FromRemoteEndpoint(m_hSocket);
	m_LocalEndpoint.FromLocalEndpoint(m_hSocket);

	if(OSSetNonBlock(m_hSocket) == false)
	{
		OSCloseSocket(m_hSocket);
		m_hSocket = INVALID_SOCKET;
		return false;
	}

	m_pInput = new Buffer();
	m_pOutput =	new Buffer();

	m_nState = kConnected;

	return true;
}

bool TcpConnection::AttachTo(TcpConnection* pConnection, bool bDelete)
{
	if(m_hSocket != INVALID_SOCKET || m_nState != kClosed || 
		pConnection == NULL || pConnection->m_hSocket == INVALID_SOCKET || 
       pConnection->m_nState == kClosed )
	{
		return false;
	}

	m_hSocket =	pConnection->m_hSocket;
	m_nState = pConnection->m_nState;
	m_LocalEndpoint = pConnection->m_LocalEndpoint;
	m_RemoteEndpoint= pConnection->m_RemoteEndpoint;

	m_pInput = new Buffer();
	m_pOutput =	new Buffer();
	m_pInput->swap(pConnection->m_pInput);
	m_pOutput->swap(pConnection->m_pOutput);

	pConnection->m_hSocket = INVALID_SOCKET;
	pConnection->m_nState = kClosed;
    
	if(bDelete)
	{
		delete pConnection;
		pConnection = NULL;
	}

	return true;
}

void TcpConnection::Close()
{
	if(m_hSocket != INVALID_SOCKET)
	{
		OSCloseSocket(m_hSocket);
		m_hSocket = INVALID_SOCKET;
	}

	m_nState = kClosed;

	if(m_pInput != NULL)
	{
		delete m_pInput;
		m_pInput = NULL;
	}

	if(m_pOutput != NULL)
	{
		delete m_pOutput;
		m_pOutput =	NULL;
	}
}

bool TcpConnection::OnRun()
{
	return true;
}

bool TcpConnection::OnConnected()
{
    SetState(kConnected);
	m_RemoteEndpoint.FromRemoteEndpoint(m_hSocket);
	m_LocalEndpoint.FromLocalEndpoint(m_hSocket);
    
	return true;
}

bool TcpConnection::OnRead()
{
	if ( m_hSocket == INVALID_SOCKET )	return false;
    m_pInput->receive(m_hSocket);
	return true;
}

bool TcpConnection::OnWrite()
{
    if ( m_hSocket == INVALID_SOCKET )	return false;
    m_pOutput->send(m_hSocket);
    return true;
}

void TcpConnection::OnDropped(int nError)
{
	return;
}

