#ifndef CONNECTIONHANDLER_H_
#define CONNECTIONHANDLER_H_

#include "ServerHandler.h"
#include "Event.hpp"
#include "MessageParser.h"
#include "TaskHandler.h"
//-----------------------------------------------------------------------------------------------------------------
//---------------------------------------------CConnectionHandler--------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------

class CConnectionHandler
{
public:

	CConnectionHandler( CServerHandler& Handler );
	
	~CConnectionHandler();

private:
	
	CConnectionHandler( const CConnectionHandler& );
	CConnectionHandler& operator=( const CConnectionHandler& );


	CEvent m_CloseEv;
	
	HANDLE m_hListenThread;
	
	static unsigned _stdcall fnListenThread( void* );
	
	CServerHandler m_ServerHandler;
	
	CTaskHandler m_TaskHandler;
	
	CMessageParser m_MessageParser;
};


#endif /*CONNECTIONHANDLER_H_*/
