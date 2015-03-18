//
// endpoint.h
// Wrapper of IP address
//

#ifndef	__ENDPOINT_H__
#define	__ENDPOINT_H__

#include "os.h"

class Endpoint
{
public:
	Endpoint();
	Endpoint& operator = (const Endpoint& endpint);
	virtual ~Endpoint();
	
	void Set(const char* host = NULL, unsigned short port = 0);
	void Set(struct sockaddr_in addr);
	void Set(unsigned long host, unsigned short port);
    
	void SetHost(const char* host);
	void SetHost(unsigned long host);
	void SetPort(unsigned short port);
    
    void FromLocalEndpoint(SOCKET sock);
 	void FromRemoteEndpoint(SOCKET sock);

	unsigned long GetHost()	const;
	const char* GetHostString()	const;
	unsigned short GetPort() const;
    
    sockaddr_in GetSockAddr() const;
    int	GetSockAddrLen() const;

	int ToString(char* s) const;
    
	static const char* GetLocalHost();
	static const in_addr GetLocalInAddr();
	static char** GetLocalHostList(int* count);
 	static const in_addr* GetLocalInAddrList(int* count);

	friend bool operator < (const Endpoint& left, const Endpoint& right);
	friend bool operator == (const Endpoint& left, const Endpoint& right);

public:
	struct sockaddr_in m_sockaddr;
	int m_socklen;
};

#endif
