#include "AsfPacker.h"

AsfPacker::AsfPacker(const std::string& fileName)
: _reader(fileName)
{
	
}

AsfPacker::~AsfPacker ()
{

}

bool AsfPacker::LocateSlice(unsigned int index)
{
    return true;
}

RtpPacket* AsfPacker::NextRtpPacket()
{
    return NULL;
}
