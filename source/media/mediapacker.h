//
// mediapacker.h
// 

#ifndef __MEDIA_PACKER_H__
#define __MEDIA_PACKER_H__

#include "os.h"

class RtpPacket;

class MediaPacker
{
public:
    virtual bool LocateSlice(size_t index) = 0; 
	virtual RtpPacket* NextRtpPacket() = 0;
};

#endif
