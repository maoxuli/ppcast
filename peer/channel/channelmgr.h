//
// channelmgr.h
//

#ifndef	__CHANNEL_MGR_H__
#define	__CHANNEL_MGR_H__

#include "channel.h"
#include "channelurl.h"
#include "osthreadlock.h"

// Channel manager owns all channels
// Thread lock inside

class ChannelMgr  
{
public:
	ChannelMgr();
	virtual ~ChannelMgr();

    // A channel is identified with a channel id
    bool StartChannel(const ChannelUrl& url);
    void StopChannel(const std::string& media);
    
    std::vector<std::string> GetChannelList();
    
    // Channel pointer is protected by reference counting
    Channel* GetChannel(const std::string& media);
    void ReleaseChannel(const std::string& media);
    
private:   
    class ChannelWrapper
    {
    public:
        ChannelWrapper(Channel* channel)
        : _channel(channel)
        , _ref(0)
        {
            
        }
        
        ~ChannelWrapper()
        {
            if(_channel != NULL)
            {
                delete _channel;
            }
        }
        
        Channel* GetChannel()
        {
            return _channel;
        }
        
        void AddRef()
		{
			_locker.Lock();
			_ref++;
			_locker.UnLock();
		}
		void Release()
		{
			_locker.Lock();
			_ref--;
			if(_ref == 0 && _channel != NULL)
            {
                delete _channel;
                _channel = NULL;
			}
			_locker.UnLock();
		}
        
    private:
        Channel* _channel;
        size_t _ref;
        OSThreadLock _locker;
    };
    
    std::map<std::string, ChannelWrapper*> _channels;
    OSThreadLock _locker;
};

extern ChannelMgr theChannelMgr;

#endif
