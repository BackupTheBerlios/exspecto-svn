#ifndef MESSAGEPARSER_H_
#define MESSAGEPARSER_H_

#include <vector>
#include "ServerHandler.h"

//-----------------------------------------------------------------------------------------------------------------
//---------------------------------------------CMessageParser------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------
class CTask;
	
class CMessageParser
{
public:

	CMessageParser( CServerHandler& Handler ):m_ServerHandler( Handler )
	{
	};
	
	virtual ~CMessageParser(){};

	SmartPtr< CTask > Parse( CInPacket& Message );
	
	typedef CTask* (*CreateTaskCallBack)( CServerHandler& Handler );

	static CreateTaskCallBack GetRegisterCreator( std::string strTaskId, CreateTaskCallBack fnCreator = NULL ); 
	
private:

	CMessageParser( const CMessageParser& );
	CMessageParser& operator=( const CMessageParser& );

	CServerHandler m_ServerHandler;

};

#endif /*MESSAGEPARSER_H_*/
