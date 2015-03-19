#ifndef	__ASFREADER_H__
#define	__ASFREADER_H__

#include "asfobject.h"
#include "asfpacket.h"
#include <string>
#include <fstream>

// A wrapper of general ASF head informations
class AsfHeader
{
public:
    AsfHeader();
    virtual ~AsfHeader();
    
    bool Initialize(const uint8_t* buf, uint16_t len);
    
    // From content description
    wchar_t*	Title;  
    wchar_t*    Author;  
    wchar_t*    Copyright;  
    wchar_t*    Description;  
    wchar_t*    Rating; 
    
    // From stream property and ex stream property
    uint16_t	AudioNum;
    uint16_t	VideoNum;
    uint32_t	AudioBitrate;
    uint32_t	VideoBitrate;
    
    // From file property
    uint64_t	Start_Time;
    uint64_t	End_Time;
    uint64_t	Duration; // ms
    uint32_t    Bitrate;
    
    uint32_t	PacketSize;
    uint64_t	PacketCount;
};

class AsfReader
{
public:
	AsfReader(const std::string& name);
	~AsfReader();
    
    bool Initialize();
    
    // To be simple
    AsfHeader Header;
    
    // Full head block
    uint64_t HeadBlock(uint8_t* buf, uint64_t n);
    
    // Locate to a asf packet
    // Asf packet is a equal size packet
    // so index and packet size can be used to locate
    bool LocatePacket(uint64_t index);
    
    // For non equal size packet file
    // offset may be used to locate
    // bool Locate(uint64_t offset);
    
    // Loop to read data packets
    AsfPacket* NextPacket();
    
private:
    std::ifstream fp;
    std::string fileName;
    
    uint64_t PacketOffset;
    uint64_t ReadCount;
};

#endif//__ASFREADER_H__
