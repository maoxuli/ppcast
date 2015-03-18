// 
// mediaslice.cpp
//

#include "mediaslice.h"
#include "rtppacket.h"

MediaSlice::MediaSlice(size_t index)
: _index(index)
, _packetCount(0)
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

bool MediaSlice::AddPacket(RtpPacket* packet)
{
	_packets.push_back(packet);
	return true;
}

void MediaSlice::Delete()
{
    
}

bool MediaSlice::ToPPPacket(PPPacket* packet)
{
    return true;
}
