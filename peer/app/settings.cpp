#include "settings.h"
#include "tinyxml.h"
#include "endpoint.h"

Settings theSettings;

Settings::Settings()
{

}

Settings::~Settings()
{
    
}

void Settings::Load()
{
	TiXmlElement*	pElement = NULL;
	TiXmlText*		pText = NULL;
	TiXmlDocument	document;
    
	if (document.LoadFile("config.xml") ) 
	{
		TiXmlHandle	hTiXml( &document );
        
		for ( pElement = hTiXml.FirstChild( "Source" ).FirstChild( "LocalDirs" ).FirstChild().Element();
             pElement != NULL;
             pElement = pElement->NextSiblingElement() )
		{
			pText = pElement->FirstChild()->ToText();
			CSTLString sDir = pText->Value();
			System.LocalDirs.push_back( sDir );
		}
        
		pText = hTiXml.FirstChild( "Source" ).FirstChild( "LocalIP" ).FirstChild().Text();
		if ( pText != NULL )
			System.Host = pText->Value();
		else
			System.Host = Endpoint::GetLocalHost();
        
		pText = hTiXml.FirstChild( "Source" ).FirstChild( "anchor_interval" ).FirstChild().Text();
		if ( pText != NULL )
			System.AnchorInterval = atoi(pText->Value());
		else
			System.AnchorInterval = DEFAULT_ANCHOR_INTERVAL;
        
	}
    
    System.Port = 5315;
	System.ClientsNumLimit = 2000;
	System.MetaVersion = 3;
}
