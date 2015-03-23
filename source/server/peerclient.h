//
// peerclient.h
//

#ifndef __PEER_CLIENT_H__
#define __PEER_CLIENT_H__

#include "TcpConnection.h"

class PPPacket;
class MediaSlicer;

class PeerClient : public TcpConnection
{
public:
    PeerClient();
    ~PeerClient();

public:
    GGUID m_nGUID;
    
    CSTLString m_sMediaName;
    
    UInt32L m_nReqNumber;

    UInt32L m_nOutPackets;

    MediaSlicer* _mediaSlicer;

    BOOL m_bRemove;

    BOOL m_bReset;

	size_t m_nSendRate;

    BOOL m_bMetaSended;

    UInt32L m_nRunCookie;

	std::map<size_t, size_t>  m_sendList;

protected:
    struct DataRequestTag
    {

        size_t m_nMin;
        size_t m_nMax;
        
        DataRequestTag(size_t nStart, size_t nEnd)
        {
            m_nMin = nStart;
            m_nMax = nEnd;
        }
    };

    typedef std::list<DataRequestTag *> DataRequestTagList;
    DataRequestTagList m_DataRequestList;
    
    UInt32L m_tLastServTime;
    UInt32L m_tLastIsNotEmtpy;

	void UpdateDataRequestList(DataRequestTag* requestTag);

protected:
    void Send(PPPacket* pPacket);
    
    virtual bool OnRun();
    virtual void OnDropped(int nError);
    virtual bool OnRead();

private:

    bool OnPacket(PPPacket* pPacket);
    bool OnClientJoin(PPPacket* pPacket);
    bool OnMetaRequest(PPPacket* pPacket);
    bool OnDataRequest(PPPacket* pPacket);
    bool OnResetRequest(PPPacket* pPacket);
    
    bool ServeDataRequest( size_t index );

    friend class PeerClientMgr;

};

#endif
