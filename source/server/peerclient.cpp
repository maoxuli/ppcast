//
// peerclient.cpp
//

#include "peerclient.h"
#include "pppacket.h"
#include "mediaslicer.h"
#include "logger.h"
#include "settings.h"

#define MAX_TIME_NO_REQUEST 1800000

PeerClient::PeerClient()
{
    m_tLastServTime	= OSMilliSeconds();
    m_nRunCookie = 0;

    m_sMediaName = "";
    _mediaSlicer = NULL;

    m_bReset = TRUE;

    m_bRemove = TRUE;

    m_bMetaSended = FALSE;
    
    m_tLastIsNotEmtpy   = OSMilliSeconds();
}

PeerClient::~PeerClient()
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

void PeerClient::Send(PPPacket* pPacket)
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

bool PeerClient::OnRead()
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

void PeerClient::OnDropped(int nError)
{
	TcpConnection::Close();
}

// 
bool PeerClient::OnRun()
{   
	if ( m_nState == kClosed )	
    {
        return false;
    }
    
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

    return true;
}

// Handle message from client to source
bool PeerClient::OnPacket(PPPacket* pPacket)
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

bool PeerClient::OnClientJoin(PPPacket* pPacket)
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
    
    if ( _mediaSlicer == NULL ) 
    {
        MediaSlicer* slicer = new MediaSlicer();
        assert(slicer != NULL);
        
        // MediaSlicer use full path name
        std::string path_name;
        std::vector<std::string> dirs = theSettings.System.LocalDirs;
        
        std::vector<std::string>::iterator it = dirs.begin();
        for ( ; it != dirs.end(); ++it )
        {
            path_name = *it;
            path_name.append( m_sMediaName );
            
            if(slicer->Initialize(path_name))
            {
                break;
            }
        }
        
        if(it == dirs.end())
        {
            // No file found
            delete slicer;
            
            PPPacket* pErrorPacket = PPPacket::New( PS_PACKET_HND_RJCT ); 
            pErrorPacket->writeString("Can not find media file error!");
            pErrorPacket->ToBuffer( m_pOutput );
            OnWrite();
			pErrorPacket->Release();
            return false;
        }

        _mediaSlicer = slicer;
    }
    
	m_nSendRate = _mediaSlicer->GetBitrate();
    PPPacket* pResPacket = PPPacket::New( PS_PACKET_HND_ALLW ); 
    pResPacket->write32u((uint32_t)m_nSendRate);
    pResPacket->ToBuffer( m_pOutput );
    OnWrite();
	pResPacket->Release();
    
    theLogger.Message(MSG_SYSTEM,"Client Join %s, Request Movie %s", m_RemoteEndpoint.GetHostString(),m_sMediaName.c_str() );
    
    return true;
}

bool PeerClient::OnDataRequest(PPPacket* pPacket)
{
    size_t nMin = pPacket->read32u();
    size_t nMax = pPacket->read32u(); 

	theLogger.Message(MSG_SYSTEM,"Get a Data Request:(%d,%d)", nMin,nMax);

	UpdateDataRequestList(new DataRequestTag(nMin, nMax));

    return true;
}

void PeerClient::UpdateDataRequestList(DataRequestTag* requestTag)
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

bool PeerClient::OnResetRequest(PPPacket* pPacket)
{
    m_bReset = true;
    m_DataRequestList.clear();
    theLogger.Message(MSG_SYSTEM,"Reset the request From %s!\r\n", m_RemoteEndpoint.GetHostString() );
    
    return true;
}

bool PeerClient::OnMetaRequest(PPPacket* pPacket)
{   
    if(m_bMetaSended == true)
    {
        return true;
    }
    
    assert(_mediaSlicer != NULL);
    size_t sliceCount = _mediaSlicer->GetSliceCount();
	SliceTable* pTable  = _mediaSlicer->GetSliceTable();
    assert(pTable != NULL);
	if ( pTable == NULL ) 
    {
        return false;
    }

    PPPacket* pOutPacket = PPPacket::New( PS_PACKET_ANS_META );
    pOutPacket->write32u( (uint32_t)sliceCount);
	for( size_t i=0; i < sliceCount; i++)
    {
		pOutPacket->write32u( (uint32_t)pTable[i].nLen );
        pOutPacket->write8u( pTable[i].bKeyFrame );
    }

    std::string sdp = _mediaSlicer->GetSDP();
	pOutPacket->writeString( sdp );

	MediaSlice* pFirstSlice = _mediaSlicer->GetSlice(0);
	if(pFirstSlice)
	{
		pOutPacket->write8u(1);

		pFirstSlice->ToBuffer(pOutPacket);

		pFirstSlice->Delete();
	}
	else
		pOutPacket->write8u(0);
    
    pOutPacket->ToBuffer( m_pOutput );	

    OnWrite();

    pOutPacket->Release();
     
    theLogger.Message(MSG_SYSTEM,"Get a Meta Request %s!", m_sMediaName.c_str() );

    m_bMetaSended = true;

    return true;
}

bool PeerClient::ServeDataRequest(size_t index)
{
    assert(_mediaSlicer != NULL);

	std::map<size_t,size_t>::iterator it = m_sendList.find( index );
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

    slice->ToBuffer( pPacket );

    pPacket->ToBuffer( m_pOutput );

    slice->Delete();

    pPacket->Release();

    OnWrite();    

	m_sendList[index] = index;
	
	theLogger.Message(MSG_SYSTEM,"Service index = %d\r\n",index);

    return true;
}
