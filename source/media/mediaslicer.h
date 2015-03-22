//
// mediaslicer.h
// SDP, SliceTable, Slice
//

#ifndef __MEDIA_SLICER_H__
#define __MEDIA_SLICER_H__

#include "mediaslice.h"
#include "os.h"

class MediaParser;
class MediaPacker;
class MediaSlice;

class MediaSlicer
{
public:
	MediaSlicer();
	virtual ~MediaSlicer();
    
	bool Initialize(const std::string& name);
    
    std::string GetSDP();
    size_t GetBitrate();
    size_t GetDuration();
    size_t GetSliceCount();
    SLICE_TABLE* GetSliceTable();
    
    MediaSlice* GetSlice(size_t index);

private:    
	MediaParser* _parser;
	MediaPacker* _packer;
};

#endif
