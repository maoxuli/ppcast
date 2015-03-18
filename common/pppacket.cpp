//
// psppacket.cpp
//

#include "pppacket.h"
#include "buffer.h"

PPPacket::PPPacketPool PPPacket::POOL;

PPPacket::PPPacket()
{
	_flag = 0;
    _type = 0;
    _seq = 0;
}

PPPacket::~PPPacket()
{

}

PPPacket* PPPacket::New(unsigned short type, unsigned int seq)
{
	PPPacket* packet = (PPPacket*)POOL.New();
	
    packet->_flag = 0;
	packet->_type = (uint16_t)type;
    packet->_seq = (uint32_t)seq;
	
	return packet;
}

PPPacket* PPPacket::FromBuffer(Buffer* buffer)
{    
	if( buffer->readable() < sizeof(HEADER) )  
    { 
        return NULL;
    }
    
    HEADER* header = (HEADER*)buffer->peek();
    size_t length = header->length;
	if( buffer->readable() < (sizeof(HEADER) + length)) 
    {
        return NULL;
    }
    header = NULL;

	PPPacket* packet = (PPPacket*)POOL.New();
    // Header
    packet->_flag = buffer->read16u();
    packet->_type = buffer->read16u();
    packet->_seq = buffer->read32u();
    length = buffer->read32u();

    // Buffer
    packet->write(buffer->read(), length);
    buffer->read(length);
    
	return packet;	
}

bool PPPacket::ToBuffer(Buffer* pBuffer) const
{
    // Header
    pBuffer->write16u(_flag);
    pBuffer->write16u(_type);
    pBuffer->write32u(_seq);
    pBuffer->write32u((uint32_t)readable());
    
    // Buffer
    pBuffer->write(read(), readable());
    
    return true;
}



