//
// mediaslice.h
//

#ifndef	__MEDIA_SLCIE_H__
#define	__MEDIA_SLCIE_H__

#include "os.h"

class RtpPacket;
class Buffer;

#pragma pack(1)
typedef struct SLICE_TABLE {
	UInt32L   nLen;
	UInt8     bKeyFrame;
} SliceTable;
#pragma pack()

// TimeSlice is tranport unit of media contents
// Current TimeSlice hold 1 second contens in RTSP packets
class MediaSlice
{
public:
	MediaSlice( size_t index );
	virtual ~MediaSlice();
    
    static MediaSlice* FromBuffer(Buffer* buffer);
    bool ToBuffer(Buffer* buffer);
    
    void Delete();
    
    size_t GetIndex();
    bool AddPacket(RtpPacket* packet);
    size_t GetLength();

private:
    size_t _index;
    std::vector<RtpPacket*> _packets;
};
#endif
