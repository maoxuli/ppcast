//
// mediaslicer.cpp
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
        assert(_parser != NULL);
        _parser->ReleasePacker(_packer);
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
    
    _packer = _parser->GetPacker();
    if(_packer == NULL)
    {
        return false;
    }
    
    return true;
}

std::string MediaSlicer::GetSDP() 
{
    assert(_parser != NULL);
    return _parser->GetSDP();
}

size_t MediaSlicer::GetBitrate()
{
    assert(_parser != NULL);
    return _parser->GetBitrate();
}

size_t MediaSlicer::GetDuration()
{
    assert(_parser != NULL);
    return _parser->GetDuration();
}

size_t MediaSlicer::GetSliceCount()
{
    assert(_parser != NULL);
    return _parser->GetSliceCount();
}

SLICE_TABLE* MediaSlicer::GetSliceTable()
{
    assert(_parser != NULL);
    return _parser->GetSliceTable();
}

MediaSlice* MediaSlicer::GetSlice(size_t index) 
{
    assert(_packer != NULL);
	assert(index < _parser->GetSliceCount());

    MediaSlice* slice = NULL;
    if(_packer->LocateSlice(index))
    {
        slice = new MediaSlice(index);
        assert(slice != NULL);

        RtpPacket* packet = _packer->NextRtpPacket();
        while(packet != NULL)
        {
            slice->AddPacket(packet);
            packet = _packer->NextRtpPacket();
        }
    }
    return slice;
}
