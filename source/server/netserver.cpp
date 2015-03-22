//
// netserver.cpp
//

#include "netserver.h"
#include "netclientmgr.h"
#include "netclient.h"
#include "endpoint.h"
#include "logger.h"
#include "settings.h"

NetServer::NetServer(const Endpoint& endpoint)
: _endpoint(endpoint)
{
	m_nAcceptedClientsCount	= 0;
	m_nSequence = 0;
	m_nProcessor = 0;
    _clientMgrList = NULL;
}

NetServer::~NetServer()
{
	Close();

    if(_clientMgrList != NULL && _processorCount > 0)
    {
        for(int i =0; i < m_nProcessor; i++)
        {
            NetClientMgr* clientMgr = _clientMgrList[i];
            delete clientMgr;
        }
    }
}

bool NetServer::Start()
{	
	if(m_hSocket != INVALID_SOCKET ) 
    {
        return false;
    }

	if(DoBind(_endpoint) == false)
	{
		theLogger.Message(MSG_DEBUG,"Server Listen Error on: %s:%d", _endpoint.GetHostString(), _endpoint.GetPort() );
		return false;
	}

    //Processor number
	_processorCount = (UInt16)OSGetNumberOfProcessors();
    assert(_processorCount > 0);
    _clientMgrList = new NetClientMgr*[_processorCount];

	for(UInt16 i =0; i < _processorCount; i++)
	{
		_clientMgrList[i] = new NetClientMgr();
		_clientMgrList[i]->StartThread();
	}

	DoListen(10);

	theLogger.Message(MSG_DEBUG,"Source Server Started...");
	theLogger.Message(MSG_DEBUG,"Source Server Listen on: %s:%d", _endpoint.GetHostString(), _endpoint.GetPort() );

	return true;	
}

bool NetServer::OnAccept()
{
	SOCKET hSocket = accept(m_hSocket, NULL, NULL);
	if(hSocket == INVALID_SOCKET)
    {
        return true;
    }

	m_nAcceptedClientsCount++;
	CreateClient(hSocket);
	return true;
}

void NetServer::CreateClient(SOCKET hSocket)
{
    NetClient* client = new NetClient();
    client->AttachTo(hSocket);

    if(client)
    { 
        UInt32L i = m_nAcceptedClientsCount % _processorCount;
        _clientMgrList[i]->AddClient(client);
    }    
}

// Count of peer client
UInt32L NetServer::GetClientCount() 
{ 
    UInt32L nTotal = 0;
    for(UInt32L i = 0; i < _processorCount; i++)
    {
        if(_clientMgrList[i] != NULL)
        {
            nTotal += _clientMgrList[i]->GetClientCount();
        }
    }

    return nTotal; 
}
