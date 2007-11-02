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

	class CListenThreadTask: public CThreadTask
	{
    public:

        CListenThreadTask( CConnectionHandler* pHandler ):m_pHandler( pHandler ){}

        virtual void Execute( const CEvent& CancelEv );

    private:

        CConnectionHandler* m_pHandler;
	};

	CThread m_ListenThread;

	CServerHandler m_ServerHandler;

	CTaskHandler m_TaskHandler;

	CMessageParser m_MessageParser;
};


#endif /*CONNECTIONHANDLER_H_*/
