//
// Thread Lock
//

#ifndef __OS_THREAD_LOCK_H__
#define __OS_THREAD_LOCK_H__

class OSThreadLock
{
public:
	OSThreadLock();
	~OSThreadLock();

public:
	void Lock();
	void UnLock();

protected:
	void InitialLock();
	void UnitialLock();

protected:
#if _OS_WINDOWS
	CRITICAL_SECTION m_pSection;
#elif _OS_LINUX
	pthread_mutex_t m_pSection;
#endif
};
#endif
