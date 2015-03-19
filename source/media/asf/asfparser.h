//
// asfparser.h
// MediaParser for ASF/WMV file
//

#ifndef __ASF_PARSER_H__
#define __ASF_PARSER_H__

#include "mediaparser.h"
#include "asfreader.h"

class AsfParser : public MediaParser
{
public:
	AsfParser(const std::string& name);
	virtual ~AsfParser();
    
    virtual bool Initialize();
    virtual std::string GetName();
	virtual std::string GetSDP();
    virtual size_t GetBitrate();
	virtual size_t GetDuration();
    virtual size_t GetSliceCount();
	virtual SliceTable* GetSliceTable();
    
    virtual MediaPacker* GetPacker();
    virtual void ReleasePacker(MediaPacker* packer);
    
protected:
    std::string _name;
	AsfReader _reader;
    
    // Parser compatibility
    unsigned short _version;
    unsigned short _system;
    
    // Slice scheme
    size_t _sliceCount;
    uint64_t* _slicePoints;
    SliceTable* _sliceTable; 
    
    std::string _sdp;
    
private:
	bool MakeIndex();
	bool MakeSDP();
	
    std::string GetPgmpu();
    std::string GetPgmpu2();
	
    // Persistent
	bool Load();
	bool Save();
};

#endif
