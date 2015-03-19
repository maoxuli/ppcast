//
// mediaslice.h
//

#ifndef	__MEDIA_SLCIE_H__
#define	__MEDIA_SLCIE_H__

#include "os.h"

class RtpPacket;
class PPPacket;

typedef struct SLICE_TABLE {
	UInt32L   nLen;
	UInt8     bKeyFrame;
	UInt8	  bAnchor;
} SliceTable;

// TimeSlice is tranport unit of media contents
// Current TimeSlice hold 1 second contens in RTSP packets
class MediaSlice
{
public:
	MediaSlice( size_t index );
	virtual ~MediaSlice();
    
    void Delete();

    bool AddPacket(RtpPacket* packet);
    size_t GetLength();
    
    bool ToPPPacket(PPPacket* packet);
    
private:
    size_t _index;
    std::vector<RtpPacket*> _packets;
};
#endif
