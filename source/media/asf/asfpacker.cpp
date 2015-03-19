#include "asfpacker.h"
#include "asfpacket.h"
#include "rtppacket.h"

AsfPacker::AsfPacker(const std::string& fileName)
: _reader(fileName)
, _sliceCount(0)
, _slicePoints(NULL)
, _readCount(0)
{
	
}

AsfPacker::~AsfPacker ()
{

}

bool AsfPacker::LocateSlice(size_t index)
{
    // Set total packet count in this slice
    assert(_slicePoints != NULL && index < _sliceCount);    
    if(index == _sliceCount - 1)
    {
        _readCount = _reader.Header.PacketCount - _slicePoints[index];
    }
    else
    {
        _readCount = _slicePoints[index + 1] - _slicePoints[index];
    }
    
    // Locate to the first packet
    return _reader.LocatePacket(_slicePoints[index]);
}

RtpPacket* AsfPacker::NextRtpPacket()
{
    if(_readCount == 0)
    {
        return NULL;
    }

    _readCount--;
    AsfPacket* asfPacket = _reader.NextPacket();
    assert(asfPacket != NULL);
    if(asfPacket == NULL)
    {
        return NULL;
        
    }
    return Packet2Rtp(asfPacket);
}

RtpPacket* AsfPacker::Packet2Rtp(AsfPacket* packet)
{
    assert(packet != NULL);
    if( packet->PadLen == 0 ) 
    {
        uint16_t rtpSize = RtpPacket::RTP_HEAD_LEN + 4 + packet->PacketLen; 
        RtpPacket* rtpPacket = new RtpPacket( rtpSize );
        uint8_t* p = rtpPacket->payload;
        
        *(uint16_t*) p = htons(0x4000);
        p += 2;
        
        *(uint16_t*) p = htons(rtpPacket->payloadSize);
        p += 2;
        
        memcpy( p, packet->Buf, packet->PacketLen );
        assert ( packet->PacketLen + 4 == rtpPacket->payloadSize );
        rtpPacket->SetTimeStamp( packet->Time );
        return rtpPacket;
    }
    else 
    {
        uint16_t rtpSize = RtpPacket::RTP_HEAD_LEN + 4 + 7 + 6 + packet->PayloadSize ; 
        RtpPacket* rtpPacket = new RtpPacket( rtpSize );
        uint8_t* buf = new uint8_t [ rtpPacket->payloadSize ];
        uint8_t* p = buf;
        
        *(uint16_t*) p = htons(0x4000);
        p += 2;
        
        *(uint16_t*) p = htons(rtpPacket->payloadSize);
        p += 2;
        
        (*p) = packet->ErrorFlags;
        p += 1;
        for(int i = 0; i < packet->ErrorDataLen; i++) 
        {
            (*p) = packet->ErrorData[i];
            p += 1;
        }
        
        (*p) = 0x40;
        if( (packet->LenFlags&0x01) != 0 ) 
        {
            (*p) |= 0x01;
        }
        p += 1;
        
        (*p) = packet->PropFlags;
        p += 1;
        
        *(uint16_t*)p =  rtpPacket->payloadSize - 4 ;
        p += 2;
        
        *(uint16_t*) p = packet->Time;
        p += 4;
        
        *(uint16_t*) p = packet->Duration;
        p += 2;
        
        memcpy( p, packet->Payload, packet->PayloadSize );
        p += packet->PayloadSize;
        
        assert( p - buf == rtpPacket->payloadSize );
        
        
        rtpPacket->SetPayLoad( buf, rtpPacket->payloadSize );
        rtpPacket->SetTimeStamp( packet->Time );
        delete buf;
        return rtpPacket;
    }
    return NULL;
}

bool AsfPacker::Initialize()
{
    return _reader.Initialize();
}

void AsfPacker::SetSlicePoints(size_t count, uint64_t* points)
{
    _sliceCount = count;
    _slicePoints = new uint64_t[count];
    memcpy(_slicePoints, points, sizeof(uint64_t) * count);
}