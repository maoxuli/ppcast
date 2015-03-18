#ifndef __ASF_PACKER_H__
#define __ASF_PACKER_H__

#include "mediapacker.h"
#include "asfreader.h"

class RtpPacket;
class AsfPacket;

//
// RTP packer for a media file
// 

class AsfPacker : public MediaPacker
{
public:
	AsfPacker(const std::string& fileName);
	virtual ~AsfPacker();
    
    bool LocateSlice(unsigned int index); 
	RtpPacket* NextRtpPacket();

private:
    AsfReader _reader;
    RtpPacket* Packet2Rtp(AsfPacket* packet); 
    
    friend class AsfParser;
};


#endif
