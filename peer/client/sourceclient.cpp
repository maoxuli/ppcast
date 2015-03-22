#include "sourceclient.h"
#include "pppacket.h"
#include "logger.h"

SourceClient::SourceClient(const Endpoint& endpoint)
: _endpoint(endpoint)
{

}


SourceClient::~SourceClient()
{
    Destroy();
}


bool SourceClient::Initialize()
{
    DoConnect(_endpoint);
    return true;
}

void SourceClient::Destroy()
{
    Close();
}

void SourceClient::OnDropped(int bError)
{
    
}

bool SourceClient::OnConnected()
{
    theLogger.Message(MSG_ERROR, "Connected.");
    TcpConnection::OnConnected();
    SendJoinRequest();
    return true;
}

bool SourceClient::OnRead()
{
    if(state() != kConnected)
    {
        return false;
    }
    
    if(!TcpConnection::OnRead())
    {
        return false;
    }

    PPPacket* pPacket = PPPacket::FromBuffer( m_pInput );
    while ( pPacket != NULL )
    {
        OnPacket(pPacket);
        pPacket->Release();
        pPacket = PPPacket::FromBuffer( m_pInput );
    }
    
    return true;
}


bool SourceClient::OnPacket(PPPacket* pPacket)
{
    switch ( pPacket->type() )
    {
    case PS_PACKET_ANS_DATA:
        theLogger.Message(MSG_ERROR,"PS_PACKET_ANS_DATA");
        return OnDataPacket(pPacket);

    case PS_PACKET_ANS_META:	
        theLogger.Message(MSG_ERROR,"PS_PACKET_ANS_META FROM SOURCE");
        return OnMetaPacket(pPacket);

    case PS_PACKET_ANS_FAIL:
        theLogger.Message(MSG_ERROR,"PS_PACKET_ANS_FAIL");
        return OnDataFailPacket(pPacket);
       
    case PS_PACKET_HND_ALLW:
        theLogger.Message(MSG_ERROR,"PS_PACKET_HND_ALLW");
        return OnAllowPacket(pPacket);

    case PS_PACKET_HND_RJCT:
        theLogger.Message(MSG_ERROR,"On Type PS_PACKET_HND_RJCT");
        return OnRejectPacket(pPacket);

    default:
        theLogger.Message(MSG_ERROR,"Error Type PN Pacekt");
        return FALSE;
    }	
}

bool SourceClient::OnDataPacket( PPPacket * pPacket )
{
    /*
    MediaSlice* pSlice = MediaSlice::FromPacket( pPacket );
	
	size_t index = pSlice->GetIndex();
	theLogger.Message(MSG_ERROR,"Get Data(Sec. %d) From SourceClient----------", index);
*/
    return true;
}

bool SourceClient::OnMetaPacket( PPPacket * pPacket )
{
    theLogger.Message(MSG_ERROR,"Meta data");
	return true;
}

bool SourceClient::OnAllowPacket( PPPacket * pPacket )
{
    theLogger.Message(MSG_ERROR,"Allow join");
    SendMetaRequest();
    return true;
}

bool SourceClient::OnRejectPacket( PPPacket * pPacket )
{
    return false;
}

bool SourceClient::OnDataFailPacket( PPPacket * pPacket )
{
    return true;
}


bool SourceClient::OnRun()
{
    return true;
}

bool SourceClient::SendDataRequest( size_t nMin, size_t nMax )
{
/*	if( m_bConnected == FALSE  ) return FALSE;

    CPSPacket* pPacket = CPSPacket::New( PS_PACKET_REQ_DATA );

    pPacket->WriteShortLE( nMin ); 
    pPacket->WriteShortLE( nMax ); 

    pPacket->ToBuffer( m_pOutput );
    pPacket->Release();
    OnWrite();

	//Log Event
	CEventLog * pEventLog = new CEventLog(DATA_REQUEST);
	pEventLog->m_chunk.nIsSource = 1;
	pEventLog->m_chunk.nFrom = nMin;
	pEventLog->m_chunk.nTo = nMax;
	pEventLog->m_chunk.nObj = m_nCookie;
	if( m_pChannel ) m_pChannel->EventReport( pEventLog );
	else delete pEventLog;		

    theApp.Message(MSG_SCHEDULE,"Request Data (%d,%d) from Source Server %s", nMin, nMax, m_sAddress);
*/
	return true;
}

bool SourceClient::SendMetaRequest( )
{
	if( state() != kConnected )  return false;
    
    PPPacket* pPacket = PPPacket::New( PS_PACKET_REQ_META );
    pPacket->writeString( "Low5MB.wmv", '\0' ); 
    pPacket->ToBuffer( m_pOutput );
	pPacket->Release();
    OnWrite();

	theLogger.Message(MSG_DEBUG,"Sent meta request");

    return true;
}


bool SourceClient::SendJoinRequest( )
{
	if( state() != kConnected  ) return false;

    PPPacket* pPacket = PPPacket::New( PS_PACKET_HND_JOIN );
    pPacket->writeString( "Low5MB.wmv", '\0'); 
    pPacket->ToBuffer( m_pOutput );
    pPacket->Release();
    OnWrite();
    return true;
}


bool SourceClient::SendResetRequest( )
{
    /*
	if( m_bConnected == FALSE  ) return FALSE;

    CPSPacket* pPacket = CPSPacket::New( PS_PACKET_REQ_RSET );
    pPacket->WriteString( m_sFileName ); 
    pPacket->ToBuffer( m_pOutput );
    pPacket->Release();

    OnWrite();
*/
    return true;
}
