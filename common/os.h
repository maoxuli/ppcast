//
// os.h
// Some system dependent functions
//

#ifndef __OS_H__
#define	__OS_H__

/////////////////////////  linux   ////////////////////////////////////

#if __linux__ || __linuxppc__ || __APPLE__

#define	_OS_LINUX 1

#include <unistd.h>
#include <sys/types.h>
#if __APPLE__
#include <sys/errno.h>
#else
#include <asm/errno.h>
#include <error.h>
#endif
#include <errno.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <assert.h>

typedef unsigned int				PointerSizedInt;
typedef unsigned char				UInt8;
typedef char						SInt8;
typedef unsigned short				UInt16;
typedef signed short				SInt16;
typedef unsigned int				UInt32;
typedef signed int					SInt32;
typedef	unsigned long				UInt32L;
typedef signed long					SInt32L;
typedef signed long long			SInt64;
typedef unsigned long long			UInt64;
typedef float						Float32;
typedef double						Float64;
typedef int							BOOL;
typedef	long						LONG;

typedef int							SOCKET;
typedef struct sockaddr_in			SOCKADDR_IN;
typedef struct sockaddr				SOCKADDR;

#ifndef INVALID_SOCKET
#define	INVALID_SOCKET				-1
#endif

#ifndef SOCKET_ERROR
#define	SOCKET_ERROR				-1
#endif

/////////////////////////  linux    ////////////////////////////////////

/////////////////////////  windows  /////////////////////////////////////
#elif _WIN32		

#define _OS_WINDOWS 1

#include <winsock2.h>
#include <windows.h>   
#include <process.h>
#include <io.h>
#include <errno.h>
#include <tchar.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>

typedef unsigned int				PointerSizedInt;
typedef unsigned char				UInt8;
typedef char						SInt8;
typedef unsigned short				UInt16;
typedef signed short				SInt16;
typedef unsigned int				UInt32;
typedef signed int					SInt32;
typedef	unsigned long				UInt32L;
typedef signed long					SInt32L;
typedef LONGLONG					SInt64;
typedef ULONGLONG					UInt64;
typedef float						Float32;
typedef double						Float64;
typedef int							BOOL;
typedef long						LONG;

#define FD_SETSIZE                  4096

#ifndef EWOULDBLOCK
#define	EWOULDBLOCK					WSAEWOULDBLOCK
#endif

#endif
/////////////////////////  windows  /////////////////////////////////////

///////////////////////// C/C++    ///////////////////////////////////////
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <wchar.h>
#include <string.h>

#ifndef BOOL
typedef SInt32	BOOL;
#endif

#ifndef TRUE
#define	TRUE	(1)
#endif

#ifndef FALSE
#define	FALSE	(0)
#endif

///////////////////////// C/C++    ////////////////////////////////////////

/////////////////////////  STL     ///////////////////////////////////////
#include <deque>
#include <map>
#include <list>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>

typedef std::list<void*>		CPointList;
typedef std::map<UInt16,void*>	CUInt16ToPointMap;
typedef std::deque<void*>		CPointQueue;
typedef std::string				CSTLString;
typedef std::wstring			CSTLWString;
typedef std::list<CSTLString>	CSTLStringList;
typedef std::vector<void*>		CPointVector;

inline bool nocase_compare(char c1, char c2 )
{
	return toupper(c1) == toupper(c2);
}

/////////////////////////  STL     //////////////////////////////////////


///////////////////////// Utility  ///////////////////////////////////////

#define MAX(a,b)            (((a) > (b)) ? (a) : (b))
#define MIN(a,b)            (((a) < (b)) ? (a) : (b))

///////////////////////// Utility  ////////////////////////////////////////


/////////////////////////  Settings  /////////////////////////////////////
#define SOCKET_SELECT_TIME_OUT_MICROSECONDS			10000
#define	TCP_LISTENERS_INTERVAL_MILLISECONDS			100
#define	TCP_CONNECTIONS_INTERVAL_MILLISECONDS		1
#define	THREAD_EXIT_TIMEOUT_MILLISECONDS			((UInt32)(500))
#define	LOCAL_SERVER_LISTEN_BACKLOG					10
#define THREAD_INTERVAL_MILLISECONDS				1
/////////////////////////  Settings  /////////////////////////////////////


/////////////////////////  App		/////////////////////////////////////
// GUID
typedef union
{
	UInt8	n[16];
	UInt8	b[16];
	UInt32L	w[4];
} GGUID;

inline bool operator==(const GGUID& guidOne, const GGUID& guidTwo)
{
	return (
            ((UInt32L*)&guidOne)[0] == ((UInt32L*)&guidTwo)[0] &&
            ((UInt32L*)&guidOne)[1] == ((UInt32L*)&guidTwo)[1] &&
            ((UInt32L*)&guidOne)[2] == ((UInt32L*)&guidTwo)[2] &&
            ((UInt32L*)&guidOne)[3] == ((UInt32L*)&guidTwo)[3] );
}

inline bool operator!=(const GGUID& guidOne, const GGUID& guidTwo)
{
	return (
            ( (UInt32L*)&guidOne)[0] != ((UInt32L*)&guidTwo)[0] ||
            ( (UInt32L*)&guidOne)[1] != ((UInt32L*)&guidTwo)[1] ||
            ( (UInt32L*)&guidOne)[2] != ((UInt32L*)&guidTwo)[2] ||
            ( (UInt32L*)&guidOne)[3] != ((UInt32L*)&guidTwo)[3] );
}

#define PPCAST_PEER_VERSION_STRING		"0.1"

#define PPCAST_SOURCE_VERSION_STRING		"0.1"
#define SYSTEM_ID_STRING "Linux"

#define PPCAST_SOURCE_VERSION 100
#define SYSTEM_ID 0


#define DEFAULT_ANCHOR_INTERVAL 60
//////////////////////////////////////////////////////////////////////////

void OSSleep( UInt32 nMilliseconds );

bool OSSetNonBlock(SOCKET hSocket);

void OSCloseSocket(SOCKET hSocket);

UInt32 OSGetLastError();

bool OSInitialSocket();

bool OSUninitialSocket();

UInt32L OSMilliSeconds();

UInt32L OSGetNumberOfProcessors();

#endif
