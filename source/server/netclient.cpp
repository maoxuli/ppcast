//
// netclient.cpp
//

#include "netclient.h"
#include "pppacket.h"
#include "mediaslicer.h"
#include "logger.h"

#define DATASERVICE_INTERVAL 1000
#define MAX_TIME_NO_REQUEST 1800000

NetClient::NetClient()
{
    m_tLastServTime	= OSMilliSeconds();
    m_nRunCookie = 0;

    m_sMediaName = "";
    _mediaSlicer = NULL;

    m_bReset = TRUE;

    m_bRemove = TRUE;

    m_bMetaSended = FALSE;
    
    //m_mInput.pLimit     = m_mOutput.pLimit = &Settings.Bandwidth.Request;

    m_tLastIsNotEmtpy   = OSMilliSeconds();
}

NetClient::~NetClient()
{
    if(m_hSocket != INVALID_SOCKET)
    {
        Close();
    }
    assert(m_hSocket == INVALID_SOCKET);

	if(_mediaSlicer != NULL)
    {
        delete _mediaSlicer;
    }

	m_sendList.clear();
}

void NetClient::Send(PPPacket* pPacket)
{
    assert(m_hSocket != INVALID_SOCKET);

    if(pPacket != NULL)
    {
        pPacket->ToBuffer(m_pOutput);
    }	

    OnWrite();

    m_nOutPackets++;

    pPacket->Release();
}

bool NetClient::OnRead()
{
    bool bSuccess = true;

    if(!TcpConnection::OnRead()) return false;

    PPPacket* pPacket = NULL;

    while(true)
    {
        pPacket = PPPacket::FromBuffer(m_pInput);
        if(pPacket == NULL)
        {
            break;
        }
        
        OnPacket(pPacket);
        pPacket->Release();
    }

    return bSuccess;
}

void NetClient::OnDropped(int nError)
{
	TcpConnection::Close();
}

// 
bool NetClient::OnRun()
{   
	if ( m_nState == kClosed )	
    {
        return false;
    }

	//if( m_bReset ) 
	//{
	//	m_nSendRate = m_nSendRate * 2;
	//}

	if(m_DataRequestList.empty()) 
    {  
        UInt32L tNow = OSMilliSeconds();

        if(tNow - m_tLastIsNotEmtpy > MAX_TIME_NO_REQUEST)
        {
            theLogger.Message(MSG_SYSTEM,"Client %s left or need nothing\r\n", m_RemoteEndpoint.GetHostString() );
            return false;
        }
    
        return true;
    }
    
    m_tLastIsNotEmtpy  = OSMilliSeconds();


	while(!m_DataRequestList.empty() && m_pOutput->readable() <= m_nSendRate)
    {
        DataRequestTag* tempDataRequest = (DataRequestTag*)m_DataRequestList.front();
		
        int nMin, nMax, n;
        n = nMin = tempDataRequest->m_nMin;
        nMax = tempDataRequest->m_nMax;

        if(nMin < nMax)  
        {
            tempDataRequest->m_nMin++;
        }
        else 
        {
            m_DataRequestList.pop_front();
        }

        if(!ServeDataRequest(n)) 
        {
            PPPacket* pErrorPacket = PPPacket::New(PS_PACKET_ANS_FAIL); 
            pErrorPacket->write16u(n);
            pErrorPacket->writeString(m_sMediaName);
            pErrorPacket->ToBuffer(m_pOutput);
            OnWrite();  
			pErrorPacket->Release();
            return false;
        }     
    }

	//if( m_bReset ) 
	//{
	//	m_nSendRate = m_nSendRate  / 2;
	//	m_bReset = FALSE;
	//}

    return TRUE;
}

// Handle message from client to source
bool NetClient::OnPacket(PPPacket* pPacket)
{
    switch(pPacket->type())
    {
    case PS_PACKET_HND_JOIN:
        return OnClientJoin(pPacket);        

    case PS_PACKET_REQ_META:
        return OnMetaRequest(pPacket);

    case PS_PACKET_REQ_DATA:			
        return OnDataRequest(pPacket);
        
    case PS_PACKET_REQ_RSET:			
        return OnResetRequest(pPacket);
        
    default:
        return false;
    }	
}

bool NetClient::OnClientJoin(PPPacket* pPacket)
{
    if(pPacket == NULL)
    {
        return false;
    }
    assert(pPacket != NULL);
        
    std::string mediaName;
    if(!pPacket->readString(mediaName, '\0') || mediaName.empty())
    {
        assert(false);
        return false;
    }

    
#if _OS_WINDOWS
	std::string::size_type idx = mediaName.find( '/' );
	while ( idx != std::string::npos )
	{
		mediaName.replace( idx, 1, "\\" );
		idx = mediaName.find( '/' );
	}
#endif
    
    if (m_sMediaName.empty()) 
    {
        m_sMediaName = mediaName ;
    }
    else
    {
		if ( m_sMediaName.size() != mediaName.size() || 
			! equal( m_sMediaName.begin(), m_sMediaName.end(), mediaName.begin(), nocase_compare) )
        {
            PPPacket* pErrorPacket = PPPacket::New( PS_PACKET_HND_RJCT ); 
            pErrorPacket->writeString("Media name error!");
            pErrorPacket->ToBuffer( m_pOutput );
            OnWrite();
			pErrorPacket->Release();
            return false;
        }
    }
    
    /*if ( _mediaSlicer == NULL ) 
    {
        _mediaSlicer = new MediaSlicer(m_sMediaName);
        assert(_mediaSlicer != NULL);
        if ( _mediaSlicer == NULL ) 
        {
            PSPacket* pErrorPacket = PSPacket::New( PS_PACKET_HND_RJCT ); 
            pErrorPacket->writeString("Can not find media file error!");
            pErrorPacket->ToBuffer( m_pOutput );
            OnWrite();
			pErrorPacket->Release();
            return false;
        }
    }
    
	m_nSendRate = _mediaSlicer->GetBitrate();
	m_mOutput.pLimit = &m_nSendRate;
    */
    PPPacket* pResPacket = PPPacket::New( PS_PACKET_HND_ALLW ); 
    pResPacket->write32u(1000);
    pResPacket->ToBuffer( m_pOutput );
    OnWrite();
	pResPacket->Release();
    
    theLogger.Message(MSG_SYSTEM,"Client Join %s, Request Movie %s", m_RemoteEndpoint.GetHostString(),m_sMediaName.c_str() );
    
    return true;
}

bool NetClient::OnDataRequest(PPPacket* pPacket)
{
    size_t nMin = pPacket->read16u();
    size_t nMax = pPacket->read16u(); 

	theLogger.Message(MSG_SYSTEM,"Get a Data Request:(%d,%d)", nMin,nMax);

	UpdateDataRequestList(new DataRequestTag(nMin, nMax));

    m_bReset= false;
    return true;
}

void NetClient::UpdateDataRequestList(DataRequestTag* requestTag)
{
    DataRequestTagList::iterator it = m_DataRequestList.begin(); 
   
    if(it != m_DataRequestList.end())
    {
        DataRequestTag* tag = *it; 

        if(requestTag->m_nMax <= tag->m_nMin) 
        {
            m_DataRequestList.push_front(requestTag);
            return;
        }
    }

    m_DataRequestList.push_back(requestTag);

}

bool NetClient::OnResetRequest(PPPacket* pPacket)
{
    m_bReset = true;
    m_DataRequestList.clear();
    theLogger.Message(MSG_SYSTEM,"Reset the request From %s!\r\n", m_RemoteEndpoint.GetHostString() );
    
    return true;
}

bool NetClient::OnMetaRequest(PPPacket* pPacket)
{   
    if(m_bMetaSended == true)
    {
        return true;
    }
    /*
    assert(_mediaSlicer != NULL);
    
    PSPacket* pOutPacket = PSPacket::New( PS_PACKET_ANS_META );

	size_t sliceCount = _mediaSlicer->GetSliceCount();

    pOutPacket->write32u( sliceCount );

	size_t bitrate = _mediaSlicer->GetBitrate();

	pOutPacket->write32u( bitrate );


	SLICE_TABLE* pTable  = _mediaSlicer->GetSliceTable();
    assert(pTable != NULL);
	if ( pTable == NULL ) return false;

	for( size_t i=0; i < sliceCount; i++)
    {
		pOutPacket->write32u( pTable[i].nLen );
        pOutPacket->write8u( pTable[i].bKeyFrame );
        pOutPacket->write8u( pTable[i].bAnchor );
    }

    std::string sdp = _mediaSlicer->GetSDP();
	pOutPacket->writeString( sdp );

	MediaSlice* pFirstSlice = _mediaSlicer->GetSlice(0);
	if(pFirstSlice)
	{
		pOutPacket->write8u(1);

		pFirstSlice->ToPSPacket(pOutPacket);

		pFirstSlice->Delete();
	}
	else
		pOutPacket->write8u(0);
     */
    
    PPPacket* pOutPacket = PPPacket::New( PS_PACKET_ANS_META );

    pOutPacket->ToBuffer( m_pOutput );	

    OnWrite();

    pOutPacket->Release();
     
    theLogger.Message(MSG_SYSTEM,"Get a Meta Request %s!", m_sMediaName.c_str() );

    m_bMetaSended = true;

    return true;
}

bool NetClient::ServeDataRequest(size_t index)
{
    assert(_mediaSlicer != NULL);

	std::map<UInt16,UInt16>::iterator it = m_sendList.find( index );
	if( it != m_sendList.end() ) 
    {
        return true;
    }

    MediaSlice* slice = _mediaSlicer->GetSlice( index );

    if( slice == NULL ) 
    { 
        return false;
    }

    PPPacket* pPacket = PPPacket::New( PS_PACKET_ANS_DATA );

    slice->ToPPPacket( pPacket );

    pPacket->ToBuffer( m_pOutput );

    slice->Delete();

    pPacket->Release();

    OnWrite();    

	m_sendList[index] = index;
	
	theLogger.Message(MSG_SYSTEM,"Service index = %d\r\n",index);

    return true;
}
