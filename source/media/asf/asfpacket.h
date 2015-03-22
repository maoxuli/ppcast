#ifndef	__ASFPACKET_H__
#define	__ASFPACKET_H__

#include <stdint.h>
#include <ctype.h>

// AsfPacket is a tool class to parse asf data packet
// The data is passed in by user in a memory block
// AsfPacket parse the data to get necessary information
// But AsfPacket does not own the memory block of data
// User is in charge of the memory block
// The object of AsfPacket should be alwasy destroyed before
// the memory block is released.
//
class AsfPacket
{	
public:
    AsfPacket();
    ~AsfPacket();
	
    bool Initialize(const char* buf, size_t len);
    
    // AsfPacket neither duplicate the data in memory block,
    // nor delete the momory pointer.
    // But it may access the data during whole lifecycle of the object,
    // So please keep the buffer unchanged untill the object is destroyed,
    // and make sure to delete the buffer after then.
	static AsfPacket* CreatePacket(const char* buf, size_t len);
	
private:
	bool ParserPayload();
	bool ParserSinglePayload();
	bool ParserMultiplePayload();
	
public:
    // This points to a memory block hold asf packet data
    // Dont delete it, it is incharge of by users
    const char*  Buf;
    size_t Len;
	
    uint8_t     ErrorFlags;
	uint8_t     ErrorDataLen;
    const char*  ErrorData;
    
    uint8_t     LenFlags; // Length of seq, pad, packet
    uint8_t     PropFlags;
	
    uint32_t	PacketLen;
    uint32_t	Sequence;
    uint32_t	PadLen;
    uint32_t	Time;
    uint32_t	Duration;
    
    size_t        PayloadSize;
    const char*  Payload;
    
	bool	HasKeyFrame;
	bool    HasMultiple;
};

#endif
