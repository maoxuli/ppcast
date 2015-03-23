// 
// Wrapper of TCP listener
//

#ifndef __TCPLISTENER_H__
#define __TCPLISTENER_H__

#include "network.h"

class TcpListener : public Socket
{
public:
	TcpListener();
	virtual ~TcpListener();
    
    bool DoBind(const Endpoint& endpoint);    
    bool DoListen(int backlog);
    void Close();
    
};

#endif
