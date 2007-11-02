#include "precomp.h"
#include "ConnectionHandler.h"
#include <process.h>

//-----------------------------------------------------------------------------------------------------------------
//---------------------------------------------CConnectionHandler--------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------

CConnectionHandler::CConnectionHandler( CServerHandler& Handler ):m_ServerHandler( Handler )
													 			 ,m_MessageParser( Handler )
                                                                 ,m_ListenThread( SmartPtr<CListenThreadTask( this ) )
{
}

CConnectionHandler::~CConnectionHandler()
{
	try{
		Log::instance().Trace( 90, "CConnectionHandler::~CConnectionHandler: Закрытие соединения с %s", m_ServerHandler.GetServerAddress().c_str() );
		m_ServerHandler.CloseSession();
		Log::instance().Trace( 90, "CConnectionHandler::~CConnectionHandler: Уничтожение" );
	}catch( std::exception& e )
	{
		Log::instance().Trace( 10," CAgent::~CAgent: Возникло исключение: %s", e.what() );
	}catch(...)
	{
		Log::instance().Trace( 10," CAgent::ListenThread: Возникло неизвестное исключение" );
	}
}

void CConnectionHandler::CListenThreadTask::Execute( const CEvent& CancelEv )
{
	CInPacket Msg;

	try{
		Log::instance().Trace( 90, "CConnectionHandler::fnListenThread: Запуск потока ожидания входящих сообщений c адреса %s", m_pHandler->m_ServerHandler.GetServerAddress().c_str() );
		while( true )
		{
			m_pHandler->m_ServerHandler.Receive( Msg );
			Log::instance().Trace( 90, "CConnectionHandler::fnListenThread: Получен пакет" );
			//Задаем данные для разбора входящего пакета
			SmartPtr< CTask > pTask = m_pHandler->m_MessageParser.Parse( Msg );
			m_pHandler->m_TaskHandler.AddTask( pTask );
		}
		Log::instance().Trace( 90, "CConnectionHandler::fnListenThread: Закрытие потока ожидания входящих сообщений с адреса %s", m_pHandler->m_ServerHandler.GetServerAddress().c_str() );
	}catch( SocketErr& e )
	{
		Log::instance().Trace( 10," CConnectionHandler::fnListenThread: Ошибка связи: %s", e.what() );
	}catch( std::exception& e )
	{
		Log::instance().Trace( 10," CConnectionHandler::fnListenThread: Возникло исключение: %s", e.what() );
	}catch( ... )
	{
		Log::instance().Trace( 10," CConnectionHandler::fnListenThread: Возникло неизвестное исключение" );
	}
	Log::instance().Trace( 90, "CConnectionHandler::fnListenThread: Закрытие" );
}


