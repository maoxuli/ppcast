//
// channel.h
//

#ifndef	__CHANNEL_H__
#define	__CHANNEL_H__

#include "selector.h"

class SourceClient;

class Channel : public Selector
{
public:
	Channel();
	virtual ~Channel();
    
    std::string GetSDP();
    
    // Override from Selector
    virtual void OnRun();

protected:
    // Media
    
    // As a client of source server
    SourceClient* _sourceClient;
};

#endif 

