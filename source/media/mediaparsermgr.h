//
// mediaparsermge.h
//

#ifndef	__MEDIA_PARSER_MGR_H__
#define	__NEDIA_PARSER_MGR_H__

#include "mediaparser.h"
#include "osthreadlock.h"
#include "os.h"

class MediaParser;

// Single MediaParser for same file
// Reference counting management
class MediaParserMgr
{
    // External reference counting
	class ParserWrapper
	{
	public:
		ParserWrapper(MediaParser* parser)
        : _parser(parser)
        , _reference(1)
        
		{

		}
		~ParserWrapper()
		{
			if(_parser != NULL)
			{
				delete _parser;
			}
		}
        
		MediaParser* GetParser()
		{
			return _parser;
		}
        
		void AddRef()
		{
			_locker.Lock();
			_reference++;
			_locker.UnLock();
		}
		void Release()
		{
			_locker.Lock();
			_reference--;
			if(_reference == 0 && _parser != NULL)
            {
                delete _parser;
                _parser = NULL;
			}
			_locker.UnLock();
		}
        
	private:
		MediaParser* _parser;
        size_t _reference;
        OSThreadLock _locker;
	};

public:
	MediaParserMgr();
	virtual ~MediaParserMgr();

public:
	MediaParser* GetParser(const std::string& name);
	bool ReleaseParser(const std::string& name);

private:
	OSThreadLock _locker;
    std::map<std::string, ParserWrapper*> _parsers;
    
    MediaParser* CreateParser(const std::string& name);
};

extern MediaParserMgr theParserMgr;

#endif
