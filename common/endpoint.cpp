//
// endpoint.cpp
//

#include "endpoint.h"

Endpoint::Endpoint()
{
    
}

Endpoint::~Endpoint()
{
    
}

void Endpoint::Set(const char* host, unsigned short port)
{
	m_sockaddr.sin_family = AF_INET;
	m_sockaddr.sin_addr.s_addr = inet_addr(host);
	m_sockaddr.sin_port = htons(port);
	m_socklen = sizeof(m_sockaddr);
}

void Endpoint::Set(struct sockaddr_in sa)
{
	m_sockaddr = sa;
	m_socklen =	sizeof(m_sockaddr);
}

void Endpoint::Set(unsigned long host, unsigned short port)
{
	m_sockaddr.sin_family = AF_INET;
	m_sockaddr.sin_addr.s_addr = htonl(host);
	m_sockaddr.sin_port = htons(port);
	m_socklen = sizeof(m_sockaddr);
}

void Endpoint::SetHost(const char* host)
{
	m_sockaddr.sin_addr.s_addr = inet_addr(host);
}

void Endpoint::SetHost(unsigned long host)
{
	m_sockaddr.sin_addr.s_addr = htonl(host);
}

void Endpoint::SetPort(unsigned short port)
{
	m_sockaddr.sin_port = htons(port);
}

unsigned long Endpoint::GetHost() const
{
	return ntohl(m_sockaddr.sin_addr.s_addr);
}

const char*	Endpoint::GetHostString() const
{
	return inet_ntoa(m_sockaddr.sin_addr);
}

unsigned short Endpoint::GetPort() const
{
	return ntohs(m_sockaddr.sin_port);
}

sockaddr_in Endpoint::GetSockAddr() const
{
	return m_sockaddr;
}

int	Endpoint::GetSockAddrLen() const
{
	return m_socklen;
}

int Endpoint::ToString(char* str) const
{
	char* temp = inet_ntoa(m_sockaddr.sin_addr);
	int length = 0;
	length = sprintf(str, "%s", temp);
	str[length] = ':';
	length += sprintf(str + length + 1 , "%d", GetPort());
	return length;
}

void Endpoint::FromLocalEndpoint(SOCKET sock)
{
	struct sockaddr_in localAddr;
#if _OS_WINDOWS
	int	nAddrLen = sizeof(struct sockaddr_in);
#elif _OS_LINUX
	unsigned int nAddrLen = sizeof(struct sockaddr_in);
#endif
    
	if(getsockname(sock, (struct sockaddr*)&localAddr, &nAddrLen ) != 0) 
	{
#ifdef _DEBUG
		printf("getsockname error: %d...\n",  OSGetLastError());
#endif
	}
	
	this->Set(localAddr);
}

void Endpoint::FromRemoteEndpoint(SOCKET sock)
{
	struct sockaddr_in remoteAddr;
#if _OS_WINDOWS
	int	nAddrLen = sizeof(struct sockaddr_in);
#elif _OS_LINUX
	unsigned int nAddrLen = sizeof(struct sockaddr_in);
#endif
    
	if(getpeername(sock, (struct sockaddr*)&remoteAddr, &nAddrLen ) != 0) 
	{
#ifdef _DEBUG
		printf("getsockname error: %d...\n",  OSGetLastError());
#endif
	}
    
	this->Set(remoteAddr);
}

const char*	Endpoint::GetLocalHost()
{
	char name[255];
	hostent* hostinfo;
	char* ip = NULL;
	char* temp;

	if(gethostname(name, sizeof(name)) == 0)
    {
		if((hostinfo = gethostbyname(name)) != NULL)
		{
			temp = inet_ntoa(*((in_addr *)(hostinfo->h_addr)));
			ip = new char[strlen(temp)+1];
			strcpy(ip,temp);
		}
    }

	return ip;
}

char** Endpoint::GetLocalHostList(int* count)
{
	char name[255];
	hostent* pHostInfo;
	in_addr** addrPtr;
	char** result = NULL;

	*count = 0;
	if(gethostname(name, sizeof(name)) == 0)
    {
		if((pHostInfo = gethostbyname(name)) != NULL)
		{
			for(addrPtr = (struct in_addr**)pHostInfo->h_addr_list; *addrPtr; addrPtr++)
			{
				(*count)++;
			}
			result = (char**)malloc(sizeof(char*)*(*count));
			int i=0;
			for(addrPtr = (struct in_addr**)pHostInfo->h_addr_list; *addrPtr; addrPtr++,i++)
			{
				char* temp = inet_ntoa(**addrPtr);
				result[i] = new char[strlen(temp)+1];
				result[i] = strcpy(result[i],temp);
			}
		}
    }

	return result;
}

const in_addr Endpoint::GetLocalInAddr()
{
	char name[255];
	hostent* hostinfo;
	in_addr addr={0};
    
	if(gethostname(name, sizeof(name)) == 0)
    {
		if((hostinfo = gethostbyname(name)) != NULL)
        {
			addr = (*((in_addr *)(hostinfo->h_addr)));
        }
    }
    
	return addr;
}

const in_addr* Endpoint::GetLocalInAddrList(int* count)
{
	char name[255];
	hostent* pHostInfo;
	in_addr** addrPtr;
	in_addr* result = NULL;

	*count = 0;
	if(gethostname(name, sizeof(name)) == 0)
    {
		if((pHostInfo = gethostbyname(name) ) != NULL)
		{
			for(addrPtr = (struct in_addr **)pHostInfo->h_addr_list; *addrPtr; addrPtr++)
			{
				(*count)++;
			}
			result = (in_addr*)malloc(sizeof(in_addr)*(*count));
			int i=0;
			for(addrPtr = (struct in_addr **)pHostInfo->h_addr_list;*addrPtr;addrPtr++,i++)
			{
				result[i] = **addrPtr;
			}
		}
    }

    return result;
}

Endpoint& Endpoint::operator = (const Endpoint& endpoint)
{
	Set(endpoint.GetSockAddr());
	return (*this);
}

bool operator < (const Endpoint& left, const Endpoint& right)
{
	if(left.GetHost() < right.GetHost()) 
    { 
        return true;
    }
	else if(left.GetHost() > right.GetHost()) 
    { 
        return false;
    }

	if(left.GetPort() < right.GetPort()) 
    { 
        return true;
    }
	else 
    { 
        return false;
    }
}

bool operator == (const Endpoint& left, const Endpoint& right)
{
	if((left.GetHost() == right.GetHost()) 
       && (left.GetPort() == right.GetPort()))
	{
		return true;
	}
	else
	{
		return false;
	}
}
