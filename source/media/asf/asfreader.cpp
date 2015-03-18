#include "asfreader.h"
#include <assert.h>

AsfReader::AsfReader(const std::string& name)
: fileName(name)
, HeadObjSize(0)
, HeadObj(NULL)
, PacketOffset(0)
, PacketCount(0)
, PacketSize(0)
, ReadCount(0)
{

}

AsfReader::~AsfReader()
{
	if( HeadObj != NULL )
    {
        delete HeadObj;
    }

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
    HeadObjSize = obj.ObjectSize;
    fp.seekg( HeadObjSize, std::ios::beg );
    
    // Data object
    DATA_OBJECT dataObj;
    fp.read( (char*)&dataObj, sizeof( dataObj ) );
    assert( dataObj.ObjectID == DATA_OBJECT_ID );
    if(dataObj.ObjectID != DATA_OBJECT_ID)
    {
        printf("Asf read DATA_OBJECT failed.\n");
        printGUID(dataObj.ObjectID);
        printGUID(DATA_OBJECT_ID);
        return false;
    }
    
    // Total packet count
    PacketCount = dataObj.TotalDataPackets;
    
    // Offset of reading packets
    PacketOffset  = fp.tellg();
    
    return true;
}

AsfHeadObject* AsfReader::HeadObject()
{
    if(HeadObj == NULL && HeadObjSize > 0)
    {
        // Load AsfHeadObject
        uint8_t* buf;
        buf = new uint8_t[HeadObjSize];
        fp.seekg( 0, std::ios::beg );
        fp.read( (char*)buf, HeadObjSize );
        
        HeadObj = new AsfHeadObject();
        assert(HeadObj != NULL);
        bool ret = HeadObj->Initialize(buf, HeadObjSize);
        delete[] buf;
        if( !ret ) 
        {
            printf("Asf load head objects failed.\n");
            delete HeadObj;
            HeadObj = NULL;
        }
    }

    return HeadObj;
}

bool AsfReader::LocatePacket(uint64_t index)
{
    assert(fp.is_open());
    assert(index < PacketCount);
    
    fp.seekg( PacketOffset + index * PacketSize,  std::ios::beg );
    ReadCount = index;
    
	return 0;
}

AsfPacket* AsfReader::NextPacket()
{
	if( ReadCount >= PacketCount)
	{
		return NULL;
	}
	
	uint8_t* buf = new uint8_t[PacketSize];
	fp.read( (char*)buf, PacketSize );
	size_t readLen = fp.gcount();
	if( readLen != PacketSize ) 
	{
        delete[] buf;
		return  NULL;
	} 
   
    ReadCount++;
	AsfPacket* packet = AsfPacket::CreatePacket( buf, PacketSize );
    assert(packet != NULL);
    
    delete[] buf;
    return packet;    
}
