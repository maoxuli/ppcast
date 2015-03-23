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

size_t MediaSlice::GetIndex()
{
    return _index;
}

void MediaSlice::Delete()
{
    delete this;
}

MediaSlice* MediaSlice::FromBuffer(Buffer* buffer)
{
    if(buffer->readable() < sizeof(uint32_t) * 3)
    {
        return NULL;
    }
    
    size_t length = buffer->peek32u(sizeof(uint32_t) * 2);
    if(buffer->readable() < (sizeof(uint32_t) * 3 + length))
    {
        return NULL;
    }
    
    size_t index = buffer->read32u();
    size_t count = buffer->read32u();
    length = buffer->read32u();
    MediaSlice* slice = new MediaSlice(index);
    buffer->remove(length);
    
    return slice;
}

bool MediaSlice::ToBuffer(Buffer* buffer)
{
    buffer->write32u((uint32_t)_index);
    buffer->write32u((uint32_t)_packets.size());
    buffer->write32u((uint32_t)GetLength());
    
    for(std::vector<RtpPacket*>::iterator it = _packets.begin(); it != _packets.end(); ++it)
    {
        RtpPacket* rtpPacket = *it;
        buffer->write32u((uint32_t)rtpPacket->bufferSize);
        buffer->write(rtpPacket->buffer, rtpPacket->bufferSize);
    }
    
    return true;
}

bool MediaSlice::AddPacket(RtpPacket* packet)
{
    //assert(packet->GetTimeStamp() / 1000 == _index );
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


