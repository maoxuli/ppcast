#include "os.h"

void OSSleep(UInt32 nMilliseconds)
{
#if _OS_WINDOWS
	::Sleep(nMilliseconds);
#elif _OS_LINUX
	::usleep(nMilliseconds * 1000);
#endif
}

bool OSSetNonBlock(SOCKET hSocket)
{
#if _OS_WINDOWS
	UInt32L dwValue = 1;
	if(ioctlsocket(hSocket, FIONBIO, &dwValue) == SOCKET_ERROR)
	{
		return false;
	}
#elif _OS_LINUX
	long flag = fcntl(hSocket, F_GETFL);
	flag |= O_NONBLOCK;
	if(fcntl(hSocket, F_SETFL, flag) == SOCKET_ERROR)
	{
		return false;
	}
#endif
	return true;
}

void OSCloseSocket(SOCKET hSocket)
{
#if _OS_WINDOWS
	::closesocket(hSocket);
#elif _OS_LINUX
	::close(hSocket);
#endif
}

UInt32 OSGetLastError()
{
#if _OS_WINDOWS
	return ::GetLastError();
#elif _OS_LINUX
	return errno;
#endif
}

bool OSInitialSocket()
{
#if _OS_WINDOWS
	UInt16 wVersionRequested = 0x0202;
	WSADATA wsaData;

	if ( WSAStartup( wVersionRequested, &wsaData ) != 0 ) 
	{
		int nErrorNum = WSAGetLastError();
		printf("WSAStartup error: %d...\n", nErrorNum);
		return FALSE;
	}

	return TRUE;
#elif _OS_LINUX
	return	TRUE;
#endif
}

bool OSUninitialSocket()
{
#if _OS_WINDOWS
	WSACleanup();
#endif
	return TRUE;
}

UInt32L	OSMilliSeconds()
{
#if _OS_WINDOWS
	return	::GetTickCount();
#elif _OS_LINUX
	struct timeval tv;
	struct timezone tz;
	int nError = ::gettimeofday(&tv, &tz);
	if ( nError!= 0 )	return 0;
	UInt32L	nMilli = tv.tv_sec;
	nMilli *= 1000;
	nMilli += ( tv.tv_usec / 1000 );
	return nMilli;
#endif
}

UInt32L	OSGetNumberOfProcessors()
{
#if _OS_WINDOWS
	SYSTEM_INFO SystemInfo;
	GetSystemInfo(&SystemInfo);
	return SystemInfo.dwNumberOfProcessors;
#elif _OS_LINUX
    return 1;
#endif
}
