//
// asfparser.cpp
//

#include "asfparser.h"
#include "buffer.h"

AsfParser::AsfParser(const std::string& name) 
: _name(name)
, _reader(name)
{
	
}

AsfParser::~AsfParser()
{

}

bool AsfParser::Initialize()
{
    // Load from meta file first
    if(Load())
    {
        return true; 
    }
    
    // Load from media file
    if( !_reader.Initialize() ) 
    { 
        return false;
    }
    
    if( !GetMetaData() ) 
    { 
        return false;
    }

    if( !MakeIndex() ) 
    { 
        return false;
    }

    if( !MakeSDP() ) 
    { 
        return false;
    }

    // Update meta file
    Save();

    return true;
}

std::string AsfParser::GetName()
{
    return _name;
}

std::string AsfParser::GetSDP()
{
    return _sdp;
}

size_t AsfParser::GetBitrate()
{
    return _bitrate;
}

size_t AsfParser::GetDuration()
{
    return _duration;
}

size_t AsfParser::GetSliceCount()
{
    return _sliceCount;
}

SLICE_TABLE* AsfParser::GetSliceTable()
{
    return NULL;
    //return _sliceTable;
}

// Create a asf packer
// and pass necessary meta to avoid repeatly parsing
MediaPacker* AsfParser::CreatePacker()
{

}


bool AsfParser::MakeIndex()
{


	return true;
}



bool AsfParser::GetMetaData () 
{
    AsfHeadObject* head = _reader.HeadObject();
    return true;
}

bool AsfParser::MakeSDP()
{
    return true;
}

std::string AsfParser::GetPgmpu()
{
    std::string s;
	return s;
}

std::string AsfParser::PgmpuConvert(const std::string str) 
{
    std::string s;
    return s;
}

std::string AsfParser::GetPgmpu2()
{
    std::string s;
    return s;
}
             
// Load meta file
bool AsfParser::Load()
{
    std::string metaFile(_name);
    metaFile.append(".meta");
    
    // Open file
 	FILE* hFile = fopen( metaFile.c_str(), "rb" );
	if ( hFile == NULL )
    { 
        return false;
    }
    
    // Check version and system
    unsigned short nVersion = 0;
    unsigned short nSystem = 0;

    if(fread( &nVersion, 1, 2, hFile) != 2 || fread( &nSystem, 1, 2, hFile) != 2 
       || nVersion != PPCAST_SOURCE_VERSION || nSystem != SYSTEM_ID)
    {
        fclose( hFile );
        return false;
    }
    
    // Load into buffer
	Buffer buf;
	while ( true )
	{
        buf.ensure(1024);
        size_t nRet = fread(buf.write(), 1, buf.writable(), hFile);
        buf.write(nRet);
        
        if(nRet < 1024)
        {
            break;
        }
	}
	fclose( hFile );
	
    // Load from buffer

    // Rebuild SliceTable

	return true;
}

// Update meta file
bool AsfParser::Save()
{
    std::string metaFile(_name);
    metaFile.append(".meta");
    
	FILE* hFile = fopen( metaFile.c_str(), "wb" );
	if ( hFile == NULL )
    { 
        return false;
    }

    Buffer buf;
    
    // Write into buffer

	size_t nRet = fwrite( buf.read(), 1, buf.readable(), hFile );
    fclose( hFile );
    
	return (nRet == buf.readable());
}
