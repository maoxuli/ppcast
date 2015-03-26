//
// rtspsession.h
//

#ifndef __RTSP_SESSION_H__
#define __RTSP_SESSION_H__

#include "os.h"
#include "osthreadlock.h"

class RtspStream;
class RtspConnection;
class Channel;

// RTSP server using a session to keep state of a presentation
// A RtspSession using one or more streams to send media to client
// The stream may be RTP based or TCP based

class RtspSession
{  
public:
    RtspSession(const std::string& sid, const std::string& cid);
    virtual ~RtspSession();
    
    // Driven by external to run
    // if return false, the session will be deleted by external
    bool run();
    void close();
    
    // Identifier
    std::string id();
    std::string media();

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
    virtual long seek(long pos = -1);

    // Playing control
    virtual void play();
    virtual void pause();
    virtual void teardown();

protected:
    // Identifier
    std::string _sid;

    // State of session
    enum {INIT, READY, PLAYING};
    int _state;

    // One or more streams
    std::vector<RtspStream*> _streams;
    
    // Associated channel
    // Protected by reference counting
    Channel* _channel;
};

#endif 
