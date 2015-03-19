// 
// mediaslice.cpp
//

#include "mediaslice.h"
#include "pppacket.h"
#include "rtppacket.h"

MediaSlice::MediaSlice(size_t index)
: _index(index)
{ 

}

MediaSlice::~MediaSlice()
{
    for(std::vector<RtpPacket*>::iterator it = _packets.begin(); it != _packets.end(); ++it)
    {
        RtpPacket* packet = *it;
        delete packet;
    }
    _packets.clear();

}

void MediaSlice::Delete()
{
    delete this;
}

bool MediaSlice::AddPacket(RtpPacket* packet)
{
    assert(packet->GetTimeStamp() / 1000 == _index );
	_packets.push_back(packet);
	return true;
}

size_t MediaSlice::GetLength()
{
    size_t length = 0;
    for(std::vector<RtpPacket*>::iterator it = _packets.begin(); it != _packets.end(); ++it)
    {
        RtpPacket* rtpPacket = *it;
        length += sizeof(uint32_t); // length of rtp packet
        length += rtpPacket->bufferSize;
    }
    return length;
}

bool MediaSlice::ToPPPacket(PPPacket* packet)
{
    packet->write32u((uint32_t)_index);
    packet->write32u((uint32_t)_packets.size());
    packet->write32u((uint32_t)GetLength());
    
    for(std::vector<RtpPacket*>::iterator it = _packets.begin(); it != _packets.end(); ++it)
    {
        RtpPacket* rtpPacket = *it;
        rtpPacket->ToBuffer((Buffer*)packet);
    }
    return true;
}
