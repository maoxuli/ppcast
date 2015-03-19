#ifndef __ASF_PACKER_H__
#define __ASF_PACKER_H__

#include "mediapacker.h"
#include "asfreader.h"
#include "mediaslice.h"

class AsfPacket;
class RtpPacket;

//
// RTP packer for a media file
// 

class AsfPacker : public MediaPacker
{
public:
    virtual bool LocateSlice(size_t index); 
	virtual RtpPacket* NextRtpPacket();
    
private:
    friend class AsfParser;
    
    // Create and destroy by media parser
    AsfPacker(const std::string& fileName);
	virtual ~AsfPacker();
    
    bool Initialize();
    void SetSlicePoints(size_t count, uint64_t* points);
    
    AsfReader _reader;
    size_t _sliceCount;
    uint64_t* _slicePoints;
    uint64_t _readCount;
    
    RtpPacket* Packet2Rtp(AsfPacket* packet);  
};


#endif
