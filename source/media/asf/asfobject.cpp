#include "asfobject.h"
#include <assert.h>
#include <memory.h>

AsfHeadObject::AsfHeadObject()
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

AsfHeadObject::~AsfHeadObject()
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

bool AsfHeadObject::Initialize(const uint8_t* buf, uint16_t len)
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



