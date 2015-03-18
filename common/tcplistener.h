// 
// Wrapper of TCP listener
//

#ifndef __TCPLISTENER_H__
#define __TCPLISTENER_H__

#include "network.h"

class TcpListener : public Socket
{
public:
	TcpListener(void);
	virtual ~TcpListener(void);
    
    bool DoBind(const Endpoint& endpoint);    
    bool DoListen(int backlog);
    void Close();
    
};

#endif
