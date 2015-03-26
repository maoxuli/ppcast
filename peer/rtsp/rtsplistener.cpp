//
// rtsplistener.cpp
//

#include "rtsplistener.h"
#include "rtspserver.h"

RtspListener::RtspListener(RtspServer* server)
: _server(server)
{
    assert(_server != NULL);
}

RtspListener::~RtspListener()
{
    
}

// Return false will let the listening socket dropped
// Dont return false if no error occured

bool RtspListener::OnAccept()
{
    SOCKET hSocket = accept(m_hSocket, NULL, NULL);
	if(hSocket == INVALID_SOCKET)
    {
        return true;
    }
    
    assert(_server != NULL);
    _server->OnConnection(hSocket);
    return true;
}
