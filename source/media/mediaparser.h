//
// mediaparser.h
// 

#ifndef __MEDIA_PARSER_H__
#define __MEDIA_PARSER_H__

#include "mediaslice.h"
#include "os.h"

class MediaPacker;

//
// Parse static information of the media file
// Keep a single instance for a media file
//

class MediaParser
{
public:
	virtual bool Initialize() = 0;
    virtual std::string GetName() = 0;
	virtual std::string GetSDP() = 0;
    virtual size_t GetBitrate() = 0;
	virtual size_t GetDuration() = 0;
    virtual size_t GetSliceCount() = 0;
	virtual SLICE_TABLE* GetSliceTable() = 0;
    
    virtual MediaPacker* CreatePacker() = 0;
};

#endif
