//
// MediaSlicer.h
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
	MediaSlicer(const std::string& name);
	virtual ~MediaSlicer();
    
	bool Initialize();
    
    std::string GetSDP();
    size_t GetBitrate();
    size_t GetDuration();
    size_t GetSliceCount();
    SLICE_TABLE* GetSliceTable();
    
    MediaSlice* GetSlice(size_t index);

private:
    std::string _mediaName;
    
	MediaParser* _parser;
	MediaPacker* _packer;
};

#endif
