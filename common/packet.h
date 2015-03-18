//
// Packet.h
//

#ifndef	__PACKET_H__
#define __PACKET_H__

#include "buffer.h"
#include "osthreadlock.h"

// Packet
// Reference counting and pooling
// Derived Packet define header and payload structures 
class Packet : public Buffer
{
public:
	virtual void Reset();
    virtual inline void Delete() = 0;
	virtual bool ToBuffer(Buffer* pBuffer) const = 0;

public:
	inline void AddRef()
	{
		m_nReference++;
	}
	
	inline void Release()
	{
		if ( this != NULL && ! --m_nReference ) 
        {
            Delete();
        }
	}
	
	inline void ReleaseChain()
	{
		if(this == NULL) 
        {
            return;
        };
		
		for(Packet* pPacket = this ; pPacket ; )
		{
			Packet* pNext = pPacket->m_pNext;
			pPacket->Release();
			pPacket = pNext;
		}
	}
		
	friend class PacketPool;
    
protected:
    // Packet can be only create and destroy by pool
	Packet();
	virtual ~Packet();
    
    // Reference counting
    unsigned int m_nReference;
    
    // Chain in pool
	Packet*	m_pNext;
	
};

// Pool of Packets
// A list of packets array, used for allocate and free
// and a chain of free packets
class PacketPool
{
public:
	PacketPool();
	virtual ~PacketPool();

protected:
	Packet*	m_pFree;
	size_t m_nFree;
    
protected:
	OSThreadLock m_Lock;
    std::vector<Packet*> m_pPools;

protected:
	void Clear();
	void NewPool();
    
protected:
	virtual bool NewPoolImpl(int nSize, Packet*& pPool, int& nPitch) = 0;
	virtual void FreePoolImpl(Packet* pPool) = 0;
	
public:
	inline Packet* New()
	{
		m_Lock.Lock();
		
		if(m_nFree == 0) 
        { 
            NewPool();
        }
		assert(m_nFree > 0);
		
		Packet* pPacket = m_pFree;
		m_pFree = m_pFree->m_pNext;
		m_nFree--;
		
		m_Lock.UnLock();
		
		pPacket->Reset();
		pPacket->AddRef();
		
		return pPacket;
	}
	
	inline void Delete(Packet* pPacket)
	{
		assert(pPacket != NULL);
		assert(pPacket->m_nReference == 0);
		
		m_Lock.Lock();
		
		pPacket->m_pNext = m_pFree;
		m_pFree = pPacket;
		m_nFree++;
		
		m_Lock.UnLock();
	}
};

#endif 
