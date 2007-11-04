#ifndef CONNECTIONHANDLER_H_
#define CONNECTIONHANDLER_H_

#include "ServerHandler.h"
#include "Event.h"
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

	CServerHandler m_ServerHandler;

	CTaskHandler m_TaskHandler;

	CMessageParser m_MessageParser;

	class CListenThreadTask: public CThreadTask
	{
    public:

        CListenThreadTask( CConnectionHandler* pHandler ):m_pHandler( pHandler ){}

        virtual void Execute( CEvent& CancelEv );

    private:

        CConnectionHandler* m_pHandler;
	};

	CThread m_ListenThread;

};


#endif /*CONNECTIONHANDLER_H_*/
