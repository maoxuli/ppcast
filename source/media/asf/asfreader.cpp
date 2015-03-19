#include "asfreader.h"
#include <assert.h>
#include <memory.h>

AsfHeader::AsfHeader()
{
	Title = NULL;
	Author = NULL;
	Copyright = NULL;
	Description = NULL;
	Rating = NULL;
    
	AudioNum = 0;      
	VideoNum = 0;      
	AudioBitrate = 0;  
	VideoBitrate = 0;  
    
	Start_Time = 0;    
	End_Time = 0;      
	Duration = 0;      
	Bitrate = 0;     
	PacketSize = 0;    
	PacketCount = 0;  
}

AsfHeader::~AsfHeader()
{
	delete[] Title;  
	delete[] Author;  
	delete[] Copyright;  
	delete[] Description;  
	delete[] Rating; 
}

//
//	HEAD_OBJECT*        pHead;
//
//  FILE_OBJECT*        pFile;
//  STREAM_PROPERTY*	pAudioStream;
//  STREAM_PROPERTY*	pVideoStream;
//  CONTENT_DESCRIPTION* pContent;

//  EX_HEAD_OBJECT *	pExHead;
//  EX_STREAM_PROPERTY* pExAudioStream;
//  EX_STREAM_PROPERTY* pExVideoStream;
//

bool AsfHeader::Initialize(const uint8_t* buf, uint16_t len)
{
	const uint8_t* pBuf = buf;
    
	HEAD_OBJECT* pHead = (HEAD_OBJECT*) pBuf;
	pBuf += sizeof( HEAD_OBJECT );
    
	if( pHead->Object_ID != HEAD_OBJECT_ID || len != pHead->Object_Size )
	{
		return false;
	}
    
    // ASF head objects
    
	OBJECT* pObj;
	for( size_t i = 0; i < pHead->Number_of_Header_Objects; i++ )
	{
		pObj = (OBJECT*) pBuf;
		if( pObj->ObjectID == FILE_PROPERTY_ID )
		{
			FILE_OBJECT* pFile =(FILE_OBJECT*) pBuf;
			End_Time = pFile->Play_Duration/ 10000;	//100-nanosecond   changed to ms
			Start_Time = pFile->Preroll;			//ms
			Duration = End_Time - Start_Time;
			Bitrate = pFile->Maximum_Bitrate;
			PacketSize = pFile->Maximum_Data_Packet_Size;
			assert( pFile->Maximum_Data_Packet_Size == pFile->Minimum_Data_Packet_Size );
			PacketCount = pFile->Data_Packets_Count;
		}
		if( pObj->ObjectID == STREAM_PROPERTY_ID )
		{
			STREAM_PROPERTY* pStream = (STREAM_PROPERTY*) pBuf;
			if( pStream->Stream_Type == ASF_AUDIO_MEDIA )
			{
				AudioNum = pStream->Flags &0x07;
			}
			if( pStream ->Stream_Type == ASF_VIDEO_MEDIA )
			{
				VideoNum = pStream->Flags &0x07;
			}
		}
		if( pObj->ObjectID == CONTENT_DESCRIPTION_ID )
		{
			CONTENT_DESCRIPTION* pContent = (CONTENT_DESCRIPTION*) pBuf;
			const uint8_t * cur;
			cur = pBuf + sizeof(CONTENT_DESCRIPTION);
			if( pContent->Title_Length )
			{
				Title = (wchar_t*)new uint8_t[ pContent->Title_Length ];
                memcpy(Title, cur, pContent->Title_Length);
                cur += pContent->Title_Length;
			}
			if( pContent->Author_Length  )
			{
				Author = (wchar_t*)new uint8_t[ pContent->Author_Length ];
				memcpy(Author, cur, pContent->Author_Length);
				cur += pContent->Author_Length;
			}
			if( pContent->Copyright_Length )
			{
				Copyright =(wchar_t*) new uint8_t[ pContent->Copyright_Length];
				memcpy(Copyright, cur, pContent->Copyright_Length);
				cur += pContent->Copyright_Length;
			}
			if( pContent->Description_Length )
			{
				Description =(wchar_t*) new uint8_t[ pContent->Description_Length];
				memcpy(Description, cur, pContent->Description_Length);
				cur +=  pContent->Description_Length;
			}
			if( pContent->Rating_Length )
			{
				Rating = (wchar_t*)new uint8_t[ pContent->Rating_Length];
				memcpy(Rating, cur, pContent->Rating_Length); 
			}
		}
		
		pBuf += pObj->ObjectSize;
	}
    
    // ASF extension head objects
    
	pBuf = buf;
	pBuf += sizeof( HEAD_OBJECT );
	for( size_t i = 0; i < pHead->Number_of_Header_Objects; i++ )
	{
		OBJECT* pObj = (OBJECT*) pBuf;
		if( pObj ->ObjectID == EX_HEAD_OBJECT_ID )
		{
			EX_HEAD_OBJECT* pExHead = (EX_HEAD_OBJECT*) pBuf;
            
			const uint8_t * pExBuf = pBuf + sizeof(EX_HEAD_OBJECT);
			uint32_t exSize  = pExHead->Header_Extension_Data_Size;
			for( size_t ex = 0; ex < exSize; )
			{
				OBJECT * pExObj = (OBJECT*) pExBuf;
				if( pExObj->ObjectID == EX_STREAM_PROPERTY_ID )
				{
					EX_STREAM_PROPERTY * pExStream = (EX_STREAM_PROPERTY *) pExBuf;
					if( pExStream->Stream_Number == AudioNum )
					{
						AudioBitrate = pExStream->Data_Bitrate;
					}
					if( pExStream->Stream_Number == VideoNum )
					{
						VideoBitrate	= pExStream->Data_Bitrate;
					}
				}
				ex += pExObj->ObjectSize;
				pExBuf += pExObj->ObjectSize;
			}
		}
		pBuf += pObj->ObjectSize;
	}
    
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////

AsfReader::AsfReader(const std::string& name)
: fileName(name)
, PacketOffset(0)
, ReadCount(0)
{

}

AsfReader::~AsfReader()
{
	fp.clear();
	fp.close();
}

void printGUID(GUID id)
{
	printf("%0x,%0x,%0x,%0x,%0x,%0x,%0x,%0x,%0x,%0x,%0x\n", 
           id.Data1,id.Data2,id.Data3,id.Data4[0],id.Data4[1],id.Data4[2], 
           id.Data4[3],id.Data4[4],id.Data4[5],id.Data4[6],id.Data4[7]);
}

// Not parser header
// Only locate to data offset
bool AsfReader::Initialize() 
{	
    // Open file
	fp.open( fileName.c_str(), std::ios::in|std::ios::binary );
	if ( !fp.is_open() )  
    {
        printf("Asf open file failed.\n");
        return false;
    }
    
    // Head Object
    OBJECT obj;
    fp.read( (char*)&obj, sizeof(obj) );
    assert(obj.ObjectID == HEAD_OBJECT_ID);
    if(obj.ObjectID != HEAD_OBJECT_ID) 
    {
        printf("Asf read HEAD_OBJECT failed.\n");
        printGUID(obj.ObjectID);
        printGUID(HEAD_OBJECT_ID);
        return false;
    }
    
    // Jump to data head object
    uint64_t HeadObjSize = obj.ObjectSize;
    uint8_t* buf;
    buf = new uint8_t[HeadObjSize];
    fp.seekg( 0, std::ios::beg );
    fp.read( (char*)buf, HeadObjSize );
    
    bool ret = Header.Initialize(buf, HeadObjSize);
    delete[] buf;
    if( !ret ) 
    {
        printf("Asf load head objects failed.\n");
    }
    
    // Data object
    DATA_OBJECT DataObj;
    fp.read( (char*)&DataObj, sizeof( DataObj ) );
    assert( DataObj.ObjectID == DATA_OBJECT_ID );
    if(DataObj.ObjectID != DATA_OBJECT_ID)
    {
        printf("Asf read DATA_OBJECT failed.\n");
        printGUID(DataObj.ObjectID);
        printGUID(DATA_OBJECT_ID);
        return false;
    }
    assert(DataObj.TotalDataPackets == Header.PacketCount);
    
    // Offset of reading packets
    PacketOffset  = fp.tellg();
    
    return true;
}

uint64_t AsfReader::HeadBlock(uint8_t* buf, uint64_t n)
{
    if(buf == NULL)
    {
        return PacketOffset;
    }
    
    assert(n >= PacketOffset);
    fp.seekg(0, std::ios::beg );
	fp.read((char*)buf, PacketOffset);
	return fp.gcount();
}

bool AsfReader::LocatePacket(uint64_t index)
{
    assert(fp.is_open());
    assert(index < Header.PacketCount);
    
    fp.seekg( PacketOffset + index * Header.PacketSize,  std::ios::beg );
    ReadCount = index;
	return true;
}

AsfPacket* AsfReader::NextPacket()
{
	if( ReadCount >= Header.PacketCount)
	{
		return NULL;
	}
	
	uint8_t* buf = new uint8_t[Header.PacketSize];
	fp.read( (char*)buf, Header.PacketSize );
	size_t readLen = fp.gcount();
	if( readLen != Header.PacketSize ) 
	{
        delete[] buf;
		return  NULL;
	} 
   
    ReadCount++;
	AsfPacket* packet = AsfPacket::CreatePacket( buf, Header.PacketSize );
    assert(packet != NULL);
    
    delete[] buf;
    return packet;    
}
