#ifndef	__ASFREADER_H__
#define	__ASFREADER_H__

#include "asfobject.h"
#include "asfpacket.h"
#include <string>
#include <fstream>

class AsfReader
{
public:
	AsfReader(const std::string& name);
	~AsfReader();
    
    bool Initialize();
    
    // Head objects
    AsfHeadObject* HeadObject();
    
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
    
    uint64_t HeadObjSize;
    AsfHeadObject* HeadObj;
    
    uint64_t PacketOffset;
    uint64_t PacketCount;
    uint32_t PacketSize;
	uint64_t ReadCount;
};

#endif//__ASFREADER_H__
