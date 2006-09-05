 //------------------------------------------------------------------------------------//
//Этот файл является частью проекта Exspecto 2006г.									  
//Module: CAgent class																  
//Author: Parshin Dmitry
//Description: Класс, реализующий функции Агента
//-------------------------------------------------------------------------------------//

#pragma once
#include "stdafx.h"
#include "..\net.lib\Sockets.h"
#include ".\agent.h"
#include "windows.h"
#include "..\net.lib\Scanner.h"
#include "..\common\commands.h"
#include "..\net.lib\packet.h"

//Конструктор,strSchedulerAddress - адрес планировщика
CAgent::CAgent( std::string strSchedulerAddress ):m_CurState( enumStates::Idling )
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

	CPacket* pPacket = new CPacket();

	//Задаем данные для разбора входящего пакета
	pPacket->SetBuffer( pParams->pbBuf, pParams->iCount );
	while( pPacket->GetCommandId( bCommandId ) )
	{
		switch( bCommandId )
		{
		case GetStatus:
			BYTE pbBuf[2];
			pbBuf[0] = 0xFF;
			pbBuf[1] = pParams->pThis->m_CurState;
			//отправляем текущее состояние
			pParams->client_sock->Send( pbBuf, 2 );
			//закрываем соединение
			pParams->client_sock->Close();
			break;
		case StartScan:
			::EnterCriticalSection( &pParams->pThis->m_csCommandExec );
			//Получаем кол-во адресов в пакете
			pPacket->GetParam( iCount );

			CScanner scan;

			//::EnterCriticalSection( &pParams->pThis->m_csCurState );
			pParams->pThis->m_CurState = enumStates::Scanning;	
			//::LeaveCriticalSection( &pParams->pThis->m_csCurState );

			for( unsigned int i = 0; i < iCount; i++ )
			{
				//получаем очередной адрес и производим его сканирование
				pPacket->GetAddress( strAddress );
				scan.Scan( strAddress, List );
			}
		//	::EnterCriticalSection( &pParams->pThis->m_csCurState );
			pParams->pThis->m_CurState = enumStates::Idling;		
		//	::LeaveCriticalSection( &pParams->pThis->m_csCurState );

			std::vector< std::string >::iterator It;
			for( It = List.begin(); It != List.end(); It++ )
			{
				std::cout << *It << std::endl;
			}
			::LeaveCriticalSection( &pParams->pThis->m_csCommandExec );
			break;
		}
	}
	delete pParams->client_sock;
	delete pPacket;
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

	structAddr adr;

    //связываем серверный сокет с локальным адресом
	sock.Bind( 5000, "127.0.0.1" );
	//переводим сокет в режим прослушивания
	sock.Listen();
	//Ожидаем входящее соединение и обрабатываем его
	while( NULL != ( client_sock = sock.Accept( adr ) ) )
	{
		//принимаем соединения только от заданного планировщика
		if( ( pThis->m_strSchedulerAddress == adr.strAddr ) && ( SOCKET_ERROR != ( iCount = client_sock->Receive( pBuf, 10240 ) ) ) )
		{
			ProcessParam* params = new ProcessParam;
			params->client_sock = client_sock;
			params->iCount = iCount;
			params->pbBuf = pBuf;
			params->pThis = pThis;
			::CloseHandle( ::CreateThread( 0, 0, fnProcessThreadProc, params, 0, &dwThreadId ) );
		}
	}
	return 0;
}


int _tmain(int argc, _TCHAR* argv[])
{
	CAgent ag( "127.0.0.1" );
	Sleep( 1000000 );
	return 0;
}