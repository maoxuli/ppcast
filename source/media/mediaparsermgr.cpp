//
// mediaparsermgr.cpp
//

#include "mediaparsermgr.h"
#include "AsfParser.h"

MediaParserMgr theParserMgr;

MediaParserMgr::MediaParserMgr()
{
	_parsers.clear();
}

MediaParserMgr::~MediaParserMgr()
{
	_locker.Lock();
	for(std::map<std::string, ParserWrapper*>::iterator it = _parsers.begin(); it != _parsers.end(); ++it)
	{
		delete it->second;		
	}
	_parsers.clear();
	_locker.UnLock();
}

MediaParser* MediaParserMgr::CreateParser(const std::string& name)
{
    MediaParser* parser = NULL;
    
    std::string::size_type pos = name.rfind('.');
    if(pos != std::string::npos )
    {
        std::string ext = name.substr(pos + 1);
        
        if(ext == "asf" || ext == "wmv")
        {
            parser = new AsfParser(name);
        }
    }    
    return parser;
}

MediaParser* MediaParserMgr::GetParser(const std::string& name)
{
    MediaParser* parser = NULL;
	_locker.Lock();

    std::map<std::string, ParserWrapper*>::iterator it = _parsers.find(name);
	if(it == _parsers.end())
	{	
		parser = CreateParser(name);
		if(parser != NULL)
		{
			if(!parser->Initialize())
            {
                delete parser;
                parser = NULL;
            }
            
            ParserWrapper* wrapper = new ParserWrapper(parser);
            assert(wrapper != NULL);
            _parsers[name] = wrapper;
		}
	}
	else
	{
        ParserWrapper* wrapper = it->second;
        parser = wrapper->GetParser();
        wrapper->Increase();
	}

	_locker.UnLock();
	return parser;
}

bool MediaParserMgr::ReleaseParser(const std::string& name)
{
	bool bRet = false;
	_locker.Lock();
    
    std::map<std::string, ParserWrapper*>::iterator it = _parsers.find(name);
	if(it != _parsers.end())
	{
        ParserWrapper* wrapper = it->second;
		wrapper->Decrease();
		if(wrapper->GetParser() == NULL)
		{
			_parsers.erase(it);
            delete wrapper;
		}
		bRet = true;
	}
    
	_locker.UnLock();
	return bRet;
}
