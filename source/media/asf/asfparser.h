//
// AsfParser.h
// MediaParser for ASF/WMV file
//

#ifndef __ASF_PARSER_H__
#define __ASF_PARSER_H__

#include "MediaParser.h"
#include "AsfReader.h"

class AsfParser : public MediaParser
{
    
typedef struct _index
{
	uint64_t PackNum;
	bool  hasKeyFrame;
} INDEX;

public:
	AsfParser(const std::string& name);
	virtual ~AsfParser();
    
    virtual bool Initialize();
    virtual std::string GetName();
	virtual std::string GetSDP();
    virtual size_t GetBitrate();
	virtual size_t GetDuration();
    virtual size_t GetSliceCount();
	virtual SLICE_TABLE* GetSliceTable();
    
    virtual MediaPacker* CreatePacker();
    
protected:
    std::string _name;
	AsfReader _reader;
    
    unsigned short _version;
    unsigned short _system;
    
    size_t _sliceCount;
    // Table
    // Inex
    SLICE_TABLE* _sliceTable;
    INDEX* _sliceIndex;
    
    std::string _sdp;
    size_t _duration;
    size_t _bitrate;
    size_t _packetSize;
    
    uint64_t _packetCount;
    uint64_t _packetOffset;
    
private:
	bool MakeIndex();
	bool MakeSDP();
	bool GetMetaData();
	CSTLString GetPgmpu();
	CSTLString PgmpuConvert( const CSTLString str);
	CSTLString GetPgmpu2();

	bool Load();
	bool Save();
};

#endif//__ASFPARSER_H__
