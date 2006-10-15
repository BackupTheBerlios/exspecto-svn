 //------------------------------------------------------------------------------------//
//Этот файл является частью проекта Exspecto 2006г.									  
//Module: CAgent class																  
//Author: Parshin Dmitry
//Description: Класс, реализующий функции Агента
//-------------------------------------------------------------------------------------//
#include "agent.h"
#include "..\libNet\ServerSocket.h"
#include "constants.h"


//Конструктор,strSchedulerAddress - адрес планировщика
CAgent::CAgent( std::string strSchedulerAddress ):m_CurState( Idling )
												 ,m_strSchedulerAddress( strSchedulerAddress )
{
	DWORD dwThreadId;
	//Запускаем поток прослушивания(ожидания входящих TCP соединений)
	::CloseHandle( ::CreateThread( 0, 0, fnListenThreadProc, this, 0, &dwThreadId ) );
	::InitializeCriticalSection( &m_csCurState );
	::InitializeCriticalSection( &m_csCommandExec );
}

CAgent::~CAgent(void)
{
	::DeleteCriticalSection( &m_csCurState );
	::DeleteCriticalSection( &m_csCommandExec );
}

//Поток обработки входящих сообщений
DWORD WINAPI CAgent::fnProcessThreadProc( LPVOID pParameter )
{
	ProcessParam* pParams = (ProcessParam*)pParameter;
	BYTE bCommandId;
	DWORD iCount;
	std::string strAddress;
	std::vector< std::string > List;

	std::auto_ptr< CPacket > pPacket = std::auto_ptr< CPacket >( new CPacket() );

	//Задаем данные для разбора входящего пакета
	pPacket->SetBuffer( pParams->pbBuf, pParams->iCount );
	for(;;)
	{
		try{
			 pPacket->GetCommandId( bCommandId );
		}catch( CPacket::PacketFormatErr )
		{
			break;
		}
		switch( bCommandId )
		{
		case GET_STATUS:
			BYTE pbBuf[2];
			pbBuf[0] = 0xFF;
			pbBuf[1] = pParams->pThis->m_CurState;
			//отправляем текущее состояние
			pParams->client_sock->Send( pbBuf, 2 );
			//закрываем соединение
			pParams->client_sock->Close();
			break;
		case START_SCAN:
			::EnterCriticalSection( &pParams->pThis->m_csCommandExec );
			//Получаем кол-во адресов в пакете
			pPacket->GetParam( iCount );



			::EnterCriticalSection( &pParams->pThis->m_csCurState );
			pParams->pThis->m_CurState = Scanning;	
			::LeaveCriticalSection( &pParams->pThis->m_csCurState );

			for( unsigned int i = 0; i < iCount; i++ )
			{
				//получаем очередной адрес и производим его сканирование по всем доступным протоколам
				pPacket->GetAddress( strAddress );
				for( PluginIterator It = pParams->pThis->m_PluginContainer.begin(); It != pParams->pThis->m_PluginContainer.end(); It++ )
				{
					(*It)->Scan( strAddress, List );
				}
			}
			::EnterCriticalSection( &pParams->pThis->m_csCurState );
			pParams->pThis->m_CurState = Idling;		
			::LeaveCriticalSection( &pParams->pThis->m_csCurState );

			std::vector< std::string >::iterator It;
			for( It = List.begin(); It != List.end(); It++ )
			{
				//std::cout << *It << std::endl;
			}
			::LeaveCriticalSection( &pParams->pThis->m_csCommandExec );
			break;
		}
	}
	delete pParams->client_sock;
	delete pParams;
	return 0;
}

//Поток ожидания входящих соединений
DWORD WINAPI CAgent::fnListenThreadProc(  void* pParameter )
{
	CAgent* pThis = (CAgent*)pParameter;
	CServerSocket sock;
	CSocket* client_sock;
	BYTE pBuf[10240];
	int iCount = 0;
	DWORD dwThreadId;

	CServerSocket::structAddr adr;

    //связываем серверный сокет с локальным адресом
	sock.Bind( 5000, "127.0.0.1" );
	//переводим сокет в режим прослушивания
	sock.Listen();
	//Ожидаем входящее соединение и обрабатываем его
	while( NULL != ( client_sock = sock.Accept( adr ) ) )
	{
		try{
			//принимаем соединения только от заданного планировщика
			if( ( pThis->m_strSchedulerAddress == adr.strAddr ) && ( ( iCount = client_sock->Receive( pBuf, 10240 ) ) ) )
			{
				ProcessParam* params = new ProcessParam;
				params->client_sock = client_sock;
				params->iCount = iCount;
				params->pbBuf = pBuf;
				params->pThis = pThis;
				::CloseHandle( ::CreateThread( 0, 0, fnProcessThreadProc, params, 0, &dwThreadId ) );
			}
		}catch( CSocket::SocketErr )
		{
			//Если прислали пакет больше 10кб
			continue;
		}
	}
	return 0;
}


int main(int argc, _TCHAR* argv[])
{
	try
	{
		CAgent ag( "127.0.0.1" );
		Sleep( 1000000 );
	}catch(...)
	{
		MessageBox( NULL, "Some error occured", "Error:", MB_OK );
	}
	return 0;
}
