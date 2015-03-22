//
// asfparser.cpp
//

#include "asfparser.h"
#include "asfpacker.h"
#include "asfpacket.h"
#include "buffer.h"
#include "base64.h"
#include "rtppacket.h"
#include <sstream>

AsfParser::AsfParser(const std::string& name) 
: _name(name)
, _reader(name)
{
	_sliceCount = 0;
    _slicePoints = NULL;
    _sliceTable = NULL;
}

AsfParser::~AsfParser()
{
    if(_slicePoints != NULL)
    {
        delete [] _slicePoints;
    }
    
    if(_sliceTable != NULL)
    {
        delete [] _sliceTable;
    }
}

bool AsfParser::Initialize()
{
    // Load from meta file first
    if(Load())
    {
        return true; 
    }
    
    // Load media file
    if( !_reader.Initialize() ) 
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
    return _reader.Header.Bitrate;
}

size_t AsfParser::GetDuration()
{
    return _reader.Header.Duration;
}

size_t AsfParser::GetSliceCount()
{
    return _sliceCount;
}

SLICE_TABLE* AsfParser::GetSliceTable()
{
    return _sliceTable;
}

// Create a asf packer
// and pass necessary meta to avoid repeatly parsing
MediaPacker* AsfParser::GetPacker()
{
    AsfPacker* packer = new AsfPacker(_name);
    assert(packer != NULL);
    if(!packer->Initialize())
    {
        delete packer;
        return NULL;
    }
    
    packer->SetSlicePoints(_sliceCount, _slicePoints);
    return packer;
}

void AsfParser::ReleasePacker(MediaPacker* packer)
{
    delete packer;
}


bool AsfParser::MakeIndex()
{
    assert(_slicePoints == NULL);
    assert(_sliceTable == NULL);
    _sliceCount = _reader.Header.Duration/1000 + 16;
    _slicePoints = new uint64_t[_sliceCount];
    _sliceTable = new SliceTable[_sliceCount];

    // Keep track of packets
    size_t sec = 0;
    size_t newSec = 0;
    bool key = false;
    
    // Loop from the first packet to last one
    uint64_t index = 0;
    _slicePoints[0] = 0;
    
    size_t packetSize = _reader.Header.PacketSize;
    char* buf = new char[packetSize];
    assert(buf != NULL);
    AsfPacket* packet  = new AsfPacket();
    _reader.LocatePacket(0);
    size_t len = _reader.NextPacket(buf, packetSize);
    while(len == packetSize && packet->Initialize(buf, packetSize))
    {
        newSec = packet->Time/1000;
        assert(newSec < _sliceCount);
        
        if(newSec != sec) // Net point of slice
        {
            _slicePoints[newSec] = index;
            _sliceTable[sec].bKeyFrame = key;
            _sliceTable[sec].nLen = (index - _slicePoints[sec]) * (_reader.Header.PacketSize + RtpPacket::RTP_HEAD_LEN); 
            
            assert(newSec < (sec + 10));
            if(newSec > sec + 1)
            {
                for(size_t i = sec + 1; i < newSec; i++)
                {
                    _slicePoints[i] = _slicePoints[sec];
                    _sliceTable[i].bKeyFrame = _sliceTable[sec].bKeyFrame; 
                    _sliceTable[i].nLen = _sliceTable[sec].nLen;
                }
            }
            
            sec = newSec;
            key = false;
        }
        
        if(packet->HasKeyFrame)
        {
            key = true;
        }
        
        index++;
        len = _reader.NextPacket(buf, packetSize);
    }
    
    delete packet; 
    packet = NULL;
    delete [] buf;
    buf = NULL;
    
    assert(index == _reader.Header.PacketCount);
    //assert(sec == _sliceCount - 1);
    _sliceTable[sec].bKeyFrame = key;
    _sliceTable[sec].nLen = (index - _slicePoints[sec]) * (_reader.Header.PacketSize + RtpPacket::RTP_HEAD_LEN);
    _sliceCount = sec + 1;
 
/*#ifdef DEBUG
	for( size_t i = 0; i < _sliceCount; i++ ) 
    {
		printf("%lu %llu %u\n",i, _slicePoints[i], _sliceTable[i].bKeyFrame );		
	}	
#endif*/
    
	return true;
}

bool AsfParser::MakeSDP()
{
    std::ostringstream oss;
    oss << "v=0\r\n"; // Version: protocol version
	oss << "o=- 200512050814190250 200512050814190250 IN IP4 127.0.0.1\r\n"; // Origin: owner/creator and session identifier
    oss << "s=" << _reader.Header.Title << "\r\n"; // session name
    
	oss << "c=IN IP4 127.0.0.1\r\n"; // connection data - not required if included in all media
	oss << "b=AS:" << _reader.Header.Bitrate/1000 + 1 << "\r\n"; // bandwidth information
    
	oss << "t=0 0\r\n"; // time the session is active
    
    oss << "a=maxps:" << _reader.Header.PacketSize << "\r\n"; // zero or more session attribute lines
    oss << "a=control:rtsp://127.0.0.1:9554/1" << "\r\n";
	oss << "a=etag:{9657E110-1964-26D2-DBD8-7729E8237D58}\r\n";
    oss << "a=range:npt=" << _reader.Header.Preroll*1.0/1000 << "-" << _reader.Header.Play_Duration*1.0/1000 << "\r\n";  
	oss << "a=sendonly\r\n";
    oss << "a=pgmpu:data:application/x-wms-contentdesc," << GetPgmpu() << "\r\n";
	oss << "a=pgmpu:data:application/vnd.ms.wms-hdr.asfv1;base64," << GetPgmpu2() << "\r\n";
    
    oss << "m=audio 0 RTP/AVP 96\r\n";
    oss << "b=X-AV:" << _reader.Header.AudioBitrate/1000 + 1 << "\r\n"; // bandwidth information
	oss << "b=RS:0\r\n"; // zero or more media attribute lines
	oss << "b=RR:0\r\n";
	oss << "a=rtpmap:96 x-asf-pf/1000\r\n";
	oss << "a=control:audio\r\n";
    oss << "a=stream:" << _reader.Header.AudioNum << "\r\n";
    
	oss << "m=video 0 RTP/AVP 96\r\n";
    oss << "b=X-AV:" << _reader.Header.VideoBitrate/1000 + 1 << "\r\n";
	oss << "b=RS:0\r\n";
	oss << "b=RR:0\r\n";
	oss << "a=rtpmap:96 x-asf-pf/1000\r\n";
	oss << "a=control:video\r\n";
    oss << "a=stream:" << _reader.Header.VideoNum << "\r\n";
    
	_sdp = oss.str();
    return true;
}

std::string	AsfParser::GetPgmpu()
{
    uint64_t duration = _reader.Header.Duration;
    std::ostringstream duration_oss;
    duration_oss << duration;
    std::string duration_s = duration_oss.str();
    
    std::string::size_type pos = _name.rfind('/');
    if(pos == std::string::npos)
    {
        pos = _name.rfind('\\');
    }
    pos = std::string::npos ? 0 : pos + 1;
    std::string file_name = _name.substr(pos);
    
    std::ostringstream oss;
    oss
    << ",8,language,31,0,"
    << ",5,title,31,0,"
    << ",6,author,31,0,"
    << ",9,copyright,31,0,"
    << ",44,WMS_CONTENT_DESCRIPTION_SERVER_BRANDING_INFO,31,12,WMServer/9.0"
    << ",51,WMS_CONTENT_DESCRIPTION_PLAYLIST_ENTRY_START_OFFSET,3,4,3000"
    << ",47,WMS_CONTENT_DESCRIPTION_PLAYLIST_ENTRY_DURATION,3," << duration_s.size() << "," << duration_s
	<< ",58,WMS_CONTENT_DESCRIPTION_COPIED_METADATA_FROM_PLAYLIST_FILE,3,1,1"
	<< "42,WMS_CONTENT_DESCRIPTION_PLAYLIST_ENTRY_URL,31," << file_name.size() << "," << file_name << "%%0D%%0A";
    
	return oss.str();
}

std::string	AsfParser::GetPgmpu2()
{
    size_t len = _reader.HeadBlock(NULL, 0);
    if(len > 0)
    {
        char* buf = new char[len]; // Where it is deleted? 
        size_t n = _reader.HeadBlock(buf, len);
        
        if(n != len)
        {
            delete[] buf;
            return "";
        }
        
        char* buf2 = new char[n*4/3 + 4];
        size_t n2 = BinTo64((uint8_t*)buf, n, buf2);
        std::string s(buf2, n2);
        delete[] buf2;
        return s;
    }
    return "";
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
        size_t nRet = fread(buf.reserve(1024), 1, buf.writable(), hFile);
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

	size_t nRet = fwrite( buf.peek(), 1, buf.readable(), hFile );
    fclose( hFile );
    
	return (nRet == buf.readable());
}
