//
// Wrapper of SOCKET
//

#ifndef __SOCKET_H__
#define __SOCKET_H__		

#include "endpoint.h"

// Socket state
enum{
	kClosed,		
	kListening,
	kConnecting,
	kConnected
};

// Socket error reason
enum{
	kAcceptError,
	kReadError,
	kWriteError,
	kConnectFail
};

class Socket
{
public:
	Socket();
	virtual ~Socket();
    
    SOCKET fd()
    {
        return m_hSocket;
    }
    
    short state()
    {
        return m_nState;
    }

    virtual void SetState(int state);
    virtual void Close(); 
    
protected:
    virtual bool OnRun();
	virtual bool OnAccept();
    virtual bool OnConnected();
    virtual void OnDropped(int nError);
	virtual bool OnRead();
	virtual bool OnWrite();
	    
    friend class Selector;
    
protected:
	SOCKET m_hSocket;
	short m_nState;
};

#endif
