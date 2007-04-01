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

	std::vector< SmartPtr< CTask > > Parse( CPacket& Message );
	
	typedef CTask* (*CreateTaskCallBack)( CServerHandler& Handler );

	static CreateTaskCallBack GetRegisterCreator( int iTaskId, CreateTaskCallBack fnCreator = NULL ); 
	
private:

	CServerHandler m_ServerHandler;

};

#endif /*MESSAGEPARSER_H_*/
