//
// TcpConnection.h
//

#ifndef __TCP_CONNECTION_H__
#define __TCP_CONNECTION_H__		

#include "Network.h"

class Buffer;

class TcpConnection : public Socket
{
public:
	TcpConnection();
	virtual ~TcpConnection();
    
    bool DoBind(Endpoint ep);

public:
    // Endpoints associated with socket
	Endpoint m_LocalEndpoint;
	Endpoint m_RemoteEndpoint;

	Buffer* m_pInput;
	Buffer* m_pOutput;

public:

	virtual bool DoConnect(const Endpoint& endpoint);

	virtual bool AttachTo(SOCKET hSocket);
	virtual bool AttachTo(TcpConnection* pConnection, bool bDelete = false);

	virtual void Close();      

protected:
	virtual bool	OnRun();
	virtual bool	OnConnected();
	virtual bool	OnRead();
	virtual bool	OnWrite();
	virtual void	OnDropped(int nError);

};

#endif
