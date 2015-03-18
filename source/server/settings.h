//
// settings.h
//

#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#include "os.h"

class Settings  
{
public:
	Settings();
	virtual ~Settings();
	
    void Load();
    
public:
	struct _system
	{
        std::vector<std::string> LocalDirs;
		CSTLString		Host;
		UInt16			Port;
		UInt32			ClientsNumLimit;
		UInt16			MetaVersion;
		UInt32			AnchorInterval;
	} System;

};

extern Settings theSettings;

#endif
