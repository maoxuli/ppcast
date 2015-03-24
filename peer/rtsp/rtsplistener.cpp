//
// rtsplistener.cpp
//

#include "rtsplistener.h"
#include "rtspserver.h"

RtspListener::RtspListener(RtspServer* server)
: _server(server)
{

}

RtspListener::~RtspListener()
{
    
}

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
