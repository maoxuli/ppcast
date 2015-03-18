//
// mediaslice.h
//

#ifndef	__MEDIA_SLCIE_H__
#define	__MEDIA_SLCIE_H__

#include "os.h"

class RtpPacket;
class PPPacket;

typedef struct _slice_table {
	UInt32L   nLen;
	UInt8     bKeyFrame;
	UInt8	  bAnchor;
} SLICE_TABLE;

// TimeSlice is tranport unit of media contents
// Current TimeSlice hold 1 second contens in RTSP packets
class MediaSlice
{
public:
	MediaSlice( size_t index );
	virtual ~MediaSlice();
    
    void Delete();

private:
    size_t _index;
    
    size_t _packetCount;
    std::vector<RtpPacket*> _packets;
    

public:
    bool AddPacket(RtpPacket* packet);
    
    bool ToPPPacket(PPPacket* packet);

};
#endif
