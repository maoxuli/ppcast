#include "asfpacker.h"
#include "asfpacket.h"
#include "rtppacket.h"

AsfPacker::AsfPacker(const std::string& fileName)
: _reader(fileName)
, _sliceCount(0)
, _slicePoints(NULL)
, _readCount(0)
{
    _bufSize = 0;
    _buf = NULL;
    _packet = NULL;
}

AsfPacker::~AsfPacker ()
{
    if(_packet != NULL)
    {
        delete _packet;
    }
    
    if(_buf != NULL)
    {
        delete _buf;
    }
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
        if(_readCount == 0) // Adjcent slices are in same packet
        {
            _readCount = 1;
        }
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
    
    // First read asf packet data block
    assert(_buf != NULL);
    assert(_bufSize >= _reader.Header.PacketSize);
    size_t len = _reader.NextPacket(_buf, _bufSize);
    assert(len == _reader.Header.PacketSize);
    
    assert(_packet != NULL);
    if(!_packet->Initialize(_buf, len))
    {
        return NULL;
    }
    
    return MakeRtpPacket(_packet);
}

// RtpPacket must duplicate asf data block hold by AsfPacket
// RtpPacket has its own memory block to hold payload
RtpPacket* AsfPacker::MakeRtpPacket(AsfPacket* packet)
{
    assert(packet != NULL);
    uint16_t rtpSize = RtpPacket::RTP_HEAD_LEN + 4 + packet->Len; 
    RtpPacket* rtpPacket = new RtpPacket( rtpSize );
    uint8_t* p = rtpPacket->payload;
    
    *(uint16_t*) p = htons(0x4000);
    p += 2;
    
    *(uint16_t*) p = htons(rtpPacket->payloadSize);
    p += 2;
    
    memcpy( p, packet->Buf, packet->Len );
    assert ( packet->Len + 4 == rtpPacket->payloadSize );
    rtpPacket->SetTimeStamp( packet->Time );
    return rtpPacket;
    
    /*if( packet->PadLen == 0 ) 
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
    return NULL;*/
}

bool AsfPacker::Initialize()
{
    if(!_reader.Initialize())
    {
        return false;
    }
    
    assert(_buf == NULL);
    _bufSize = _reader.Header.PacketSize;
    _buf = new char[_bufSize];
    assert(_buf != NULL);
    
    assert(_packet == NULL);
    _packet = new AsfPacket();
    assert(_packet != NULL);
    
    return true;
}

void AsfPacker::SetSlicePoints(size_t count, uint64_t* points)
{
    _sliceCount = count;
    _slicePoints = new uint64_t[count];
    memcpy(_slicePoints, points, sizeof(uint64_t) * count);
}