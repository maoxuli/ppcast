//
// rtspsession.cpp
//

#include "rtspsession.h"
#include <sstream>

RtspSession::RtspSession(const std::string& sid, const std::string& mid)
: m_sid(sid)
, m_mid(mid)
, m_state(INIT)
{

}

RtspSession::~RtspSession()
{
    std::cout << "RtspSession::~RtspSession() " << m_sid << "\n";
}

void RtspSession::close()
{
    
}
    
std::string RtspSession::sid()
{
    return m_sid;
}

std::string RtspSession::mid()
{
    return m_mid;
}

std::string RtspSession::streamsInfo()
{
    std::ostringstream oss;

    for(std::vector<RtspStream*>::iterator it = m_streams.begin(); it != m_streams.end(); ++it)
    {
        RtspStream* p = *it;
        if(p != NULL)
        {
            oss << "url=rtsp://127.0.0.1/" << m_mid << "/" << p->name() << ";seq=" << p->seq(false) << ";rtptime=0";
        }

        if(it != m_streams.end())
        {
            oss << ",";
        }
    }

    return oss.str();
}

RtspStream* RtspSession::findStream(const std::string& name)
{
    for(std::vector<RtspStream*>::iterator it = m_streams.begin(); it != m_streams.end(); ++it)
    {
        RtspStream* p = *it;
        if(p != NULL && p->name() == name)
        {
            return p;
        }
    }

    return NULL;
}

void RtspSession::removeStream(const std::string& name)
{
    // Delete existing stream
    for(std::vector<RtspStream*>::iterator it = m_streams.begin(); it != m_streams.end(); )
    {
        RtspStream* p = *it;
        if(p != NULL && p->name() == name)
        {
            delete p;
            it = m_streams.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

// RTSession must be inited sucessfully
// Based on port, util bind a local address successfully
bool RtspSession::setupStream(const std::string& name, unsigned short& serverPort, unsigned short clientPort) 
{
    // Remove existing stream
    removeStream(name);

    // Add a new stream
    RTPStream* pStream = new RTPStream(name);
    if(pStream != NULL && pStream->init(serverPort, clientPort))
    {
        m_streams.push_back(pStream);
        m_state = READY;

        return true;
    }

    return false;
}

bool RtspSession::setupStream(const std::string& name, RtspConnection* connection)
{
    // Remove existing stream
    removeStream(name);

    // Add a new stream
    TCPStream* pStream = new TCPStream(name);
    if(pStream != NULL && pStream->init(connection))
    {
        m_streams.push_back(pStream);
        m_state = READY;

        return true;
    }

    return false;
}

void RtspSession::play()
{
    m_state = PLAYING;
}

void RtspSession::pause()
{
    m_state = READY;
}

void RtspSession::teardown()
{
    m_state = INIT;
}
