//
// MediaSlicer.cpp
//

#include "mediaslicer.h"
#include "mediaparser.h"
#include "mediapacker.h"
#include "mediaslice.h"
#include "mediaparsermgr.h"

MediaSlicer::MediaSlicer(const std::string& name)
: _mediaName(name)
{

}

MediaSlicer::~MediaSlicer()
{
    if(_packer != NULL)
    {
        delete _packer;
        _packer = NULL;
    }
    
    if(_parser != NULL)
    {
        theParserMgr.ReleaseParser(_parser->GetName());
        _parser = NULL;
    }
}

bool MediaSlicer::Initialize() 
{
    _parser = theParserMgr.GetParser(_mediaName);
    if(_parser == NULL)
    {
        return false;
    }
    
    _packer = _parser->CreatePacker();
    if(_packer == NULL)
    {
        return false;
    }
    
    return true;
}

std::string MediaSlicer::GetSDP() 
{
    return _parser->GetSDP();
}

size_t MediaSlicer::GetBitrate()
{
    return _parser->GetBitrate();
}

size_t MediaSlicer::GetDuration()
{
    return _parser->GetDuration();
}

size_t MediaSlicer::GetSliceCount()
{
    return _parser->GetSliceCount();
}

SLICE_TABLE* MediaSlicer::GetSliceTable()
{
    return _parser->GetSliceTable();
}

MediaSlice* MediaSlicer::GetSlice(size_t index) 
{
	assert(index < _parser->GetSliceCount());

	MediaSlice* slice = new MediaSlice(index);
    if(slice != NULL)
    {
        _packer->LocateSlice(index);
        
        RtpPacket* packet = _packer->NextRtpPacket();
        while(packet != NULL)
        {
            slice->AddPacket(packet);
        }
    }
    
    return slice;
}
