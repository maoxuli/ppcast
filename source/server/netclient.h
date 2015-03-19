//
// netclient.h
//

#ifndef __NET_CLIENT_H__
#define __NET_CLIENT_H__

#include "TcpConnection.h"

class PPPacket;
class MediaSlicer;

class NetClient : public TcpConnection
{
public:
    NetClient();
    ~NetClient();

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

	std::map<UInt16, UInt16>  m_sendList;

protected:
    struct DataRequestTag
    {

        UInt16 m_nMin;
        UInt16 m_nMax;
        
        DataRequestTag(UInt16 nStart, UInt16 nEnd)
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

    friend class NetClientMgr;

};

#endif
