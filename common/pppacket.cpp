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
}

PPPacket::~PPPacket()
{

}

PPPacket* PPPacket::New(unsigned short type)
{
	PPPacket* packet = (PPPacket*)POOL.New();
    // All members should be reset here
    // The base class has reset buffer though
	packet->_flag = 0;
	packet->_type = (uint16_t)type;
	return packet;
}

PPPacket* PPPacket::FromBuffer(Buffer* buffer)
{    
	if( buffer->readable() < sizeof(HEADER) )  
    { 
        return NULL;
    }
    
    PPPacket::HEADER* header = (HEADER*)buffer->peek();
    size_t length = header->length;
	if( buffer->readable() < (sizeof(HEADER) + length)) 
    {
        return NULL;
    }

	PPPacket* packet = (PPPacket*)POOL.New();
    packet->_flag = header->flag;
    packet->_type = header->type;
    buffer->remove(sizeof(PPPacket::HEADER));
    // Buffer
    packet->write(buffer->peek(0, length), length);
    buffer->remove(length);
    
	return packet;	
}

bool PPPacket::ToBuffer(Buffer* pBuffer) const
{
    PPPacket::HEADER header;
    header.flag = _flag;
    header.type = _type;
    header.length = (uint32_t)readable();
    
    pBuffer->write((uint8_t*)&header, sizeof(PPPacket::HEADER));
    
    pBuffer->write(peek(), readable());
    
    return true;
}



