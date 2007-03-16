#ifndef MESSAGEPARSER_H_
#define MESSAGEPARSER_H_

#include "ServerHandler.h"

//-----------------------------------------------------------------------------------------------------------------
//---------------------------------------------CMessageParser------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------
class CTask;
	
class CMessageParser
{
public:

	CMessageParser( CServerHandler& Handler ):m_ServerHandler( Handler )
	{};
	
	virtual ~CMessageParser(){};

	SmartPtr< CTask > TaskFactory( BYTE bCommandId, CPacket& Msg );
	
	std::vector< SmartPtr< CTask > > Parse( CPacket& Message );
	
	typedef CTask* (*CreateTaskCallBack)( CServerHandler& Handler );

	static bool RegisterCreator( int iTaskId, CreateTaskCallBack fnCreator ); 
	
private:

	CServerHandler m_ServerHandler;
	
	static std::map< int, CreateTaskCallBack > m_mapCreators;
};


#endif /*MESSAGEPARSER_H_*/
