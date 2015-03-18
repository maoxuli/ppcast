#include "packet.h"
#include "buffer.h"

Packet::Packet()
{
	m_pNext			= NULL;
	m_nReference	= 0;
}

Packet::~Packet()
{
	assert( m_nReference == 0 );
}

void Packet::Reset()
{
	assert( m_nReference == 0 );
    clear();
 	m_pNext = NULL;
}

////////////////////////////////////////////////////////////////////////////
PacketPool::PacketPool()
{
	m_pFree = NULL;
	m_nFree = 0;
}

PacketPool::~PacketPool()
{
	Clear();
}

void PacketPool::Clear()
{
	while(!m_pPools.empty())
	{
		Packet* pPool = (Packet*)m_pPools.back();
		m_pPools.pop_back();
		FreePoolImpl(pPool);
	}

	m_pFree = NULL;
	m_nFree = 0;
}

void PacketPool::NewPool()
{
	Packet* pPool = NULL;
	int nPitch = 0, nSize = 256;
	
	NewPoolImpl(nSize, pPool, nPitch);
	
	m_pPools.push_back( pPool );

	UInt8* p = (UInt8*)pPool;
	
	while ( nSize-- > 0 )
	{
		pPool = (Packet*)p;
		p += nPitch;
		
		pPool->m_pNext = m_pFree;
		m_pFree = pPool;
		m_nFree++;
	}
}


