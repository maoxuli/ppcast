//
// pppacket.h
//

#ifndef	__PP_PACKET_H__
#define __PP_PACKET_H__

#include "packet.h"

#define PACKET_UNKOWN 0x00
#define PS_PACKET_REQ_META			0x10
#define PS_PACKET_ANS_META			0x11
#define PS_PACKET_REQ_DATA			0x12
#define PS_PACKET_ANS_DATA			0x13
#define PS_PACKET_ANS_FAIL          0x14      
#define PS_PACKET_HND_JOIN          0x15 
#define PS_PACKET_HND_ALLW          0x16 
#define PS_PACKET_HND_RJCT          0x17 
#define PS_PACKET_REQ_RSET          0x18

class Buffer;

// 
// PPPacket
// A general packet with flag, type, and length as header
// The content of packet of specific type is filled by application code
//
class PPPacket : public Packet
{
public: 
#pragma pack(2)
    typedef struct {
        uint16_t flag;
        uint16_t type;
        uint32_t length;
    } HEADER;
#pragma pack()
    
protected:
    // Can only new and delete by pool
    PPPacket();
	virtual ~PPPacket();
    	
    // Packet pool of PPPacket
    class PPPacketPool : public PacketPool
    {
    public:
        virtual ~PPPacketPool()
        {
            // Clear();
        }
        
    protected:
        virtual bool NewPoolImpl(int nSize, Packet*& pPool, int& nPitch)
        {
            nPitch = sizeof(PPPacket);
            pPool = new PPPacket[ nSize ];
            return true;
        }
        
        virtual void FreePoolImpl(Packet* pPool)
        {
            delete [] (PPPacket*)pPool;
        }
    };
    
	static PPPacketPool POOL;
    friend class PPPacketPool;
    
public:
    static PPPacket* New(unsigned short type);
    static PPPacket* FromBuffer(Buffer* pBuffer);
    
    inline virtual void Delete()
	{
		POOL.Delete( this );
	}
    
    virtual bool ToBuffer(Buffer* pBuffer) const;
    
    unsigned short type()
    {
        return _type;
    }
	
protected:
    
	unsigned short _flag;
    unsigned short _type;
};

#endif 
