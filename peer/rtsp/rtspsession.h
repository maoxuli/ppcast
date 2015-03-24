//
// rtspsession.h
//

#ifndef __RTSP_SESSION_H__
#define __RTSP_SESSION_H__

#include "rtspconnection.h"
#include "rtspstream.h"


// 
// RTSP server using a session to keep state of a presentation
// A RtspSession using one or more streams to send media to client
// The stream may be RTP based or TCP based
//

class RtspSession
{  
public:
    RtspSession(const std::string& sid, const std::string& mid);
    virtual ~RtspSession();

    // Close session
    void close();
    
    // Identifier
    std::string sid();
    std::string mid();

    // Get all streams info	(for RTSP response)	
    std::string streamsInfo();

    // find a stream 
    RtspStream* findStream(const std::string& name);

    // Delet a stream
    void removeStream(const std::string& name);

    // Setup a stream
    bool setupStream(const std::string& name, unsigned short& serverPort, unsigned short clientPort);
    bool setupStream(const std::string& name, RtspConnection* conn);

    // Seek to a position, return actual result position 
    // With -1 to return current position
    virtual int seek(int pos = -1) = 0;

    // Playing control
    virtual void play();
    virtual void pause();
    virtual void teardown();
        
    // Driven by external thread
    virtual bool run() = 0;

protected:
    // Identifier
    std::string m_sid;
    std::string m_mid;

    // State of session
    enum {INIT, READY, PLAYING};
    int m_state;

    // One or more streams
    std::vector<RtspStream*> m_streams;			
};

#endif 
