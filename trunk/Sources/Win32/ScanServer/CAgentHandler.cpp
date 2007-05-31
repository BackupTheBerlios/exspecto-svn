//------------------------------------------------------------------------------------//
//Этот файл является частью проекта Exspecto 2006г.
//Module: Agent handler class
//Author: Parshin Dmitry
//Description: Класс, инкапсулирующий взаимодействие с агентом
//-------------------------------------------------------------------------------------//
#include "precomp.h"
#include "CAgentHandler.h"
#include <process.h>
#include "DbProviderFactory.h"
#include <list>
#include <algorithm>


#define RECEIVE_BUF_START_SIZE 255
#define RECEIVE_BUF_MAX_SIZE 25000000

CAgentHandler::CAgentHandler( std::string strAgentAddress ):m_strAddress( strAgentAddress )
														   ,m_ScanFinished( false )
{
	m_pConnectionHandler = SmartPtr< CConnectionHandler >( new CConnectionHandler( this ) );
	//подготавливаем приемный буфер
	m_vecRecvBuf.resize( RECEIVE_BUF_START_SIZE );
}

CAgentHandler::~CAgentHandler()
{
	try
	{
		//Перед уничтожением закрываем соединение с агентом
		Close();
	}catch( ... ){}
}

//Отправить пакет Msg агенту и получить ответ в pbRespBuf, iRespSize - ожидаемый размер ответа
enumAgentResponse CAgentHandler::SendMessage( CPacket &Msg, CReceiver& Receiver )
{
	//Команды посылаются синхронно
	CLock lock( m_csExec );
	
	char Result = -1;
	const static BYTE EndStamp[] = { 0, 0x10, 0x13, 0 };
	if( !IsOpened() )
	{
		Log::instance().Trace( 10, "CAgentHandler::SendMessage: Ошибка!Сессия не открыта!" );
		throw HandlerErr( "Session has not been opened, but SendMessage called" );
	}
	BYTE* pBuf = NULL;
	int iSize = 0;
	//Получаем буфер с данными сообщения Msg для отправки по сети
	Msg.GetBuffer( pBuf, iSize );
	Log::instance().Dump( 80, pBuf, iSize, "CAgentHandler::SendMessage: отправляем буфер:" );
	if( 0 == iSize )
		throw HandlerErr( "Attempt to send message with zero-length was made" );		
	m_Sock.Send( pBuf, iSize );

	int iCount;
	bool bEnd = false;
	//Как правило если пакет данных - промежуточный, то в конце он содержит неполную запись,
	//эта неполная запись копируется в начало приемного буфера,а iReceiveOffset - указывает на 
	//конец этой неполной записи, т.о. данные записываются корректно
	int iReceiveOffset = 0;
	//Получаем ответ на сообщение
	while( !bEnd && ( iCount = m_Sock.Receive( &m_vecRecvBuf[iReceiveOffset], (int)m_vecRecvBuf.size()-iReceiveOffset ) ) > 0 )
	{
		//Смещение от начала буфера, 0 или 1
		//1 - когда пакет первый в очереди и содержит первый байт обработки команды
		//0 - в остальных случаях
		BYTE bOffset = 0;
		if( -1 == Result )
		{
			Result = m_vecRecvBuf[0];
			bOffset = 1;
		}
		Log::instance().Trace( 80, "CAgentHandler::SendMessage: iCount = %d", iCount );
		Log::instance().Dump( 200, (BYTE*)&m_vecRecvBuf[iReceiveOffset], iCount, "CAgentHandler::SendMessage: Приняты данные:" );
		//Проверяем на конец пакета
		if( ( iCount > 4 ) && ( 0 == memcmp( EndStamp, &m_vecRecvBuf[ iReceiveOffset + iCount - 4 ], 4 ) ) )
		{
			iCount -= 4;
			bEnd = true;
		//Если получили только маркер конца
		}else if ( ( iCount == 4 ) && ( 0 == iReceiveOffset ) && ( 0 == memcmp( EndStamp, &m_vecRecvBuf[ 0 ], 4 ) ) )
			break;

		//AddData возвращает итератор, указывающий на первый элемент неполной записи(см.коментарий к iReceiveOffset)
		std::vector<BYTE>::iterator ItRecv;
		if( (m_vecRecvBuf.begin() + iReceiveOffset + iCount) != ( ItRecv = Receiver.AddData( m_vecRecvBuf.begin() + bOffset, m_vecRecvBuf.begin()+ iReceiveOffset + iCount ) ) )
		{
			//Копируем неполную запись в начало приемного буфера
			iReceiveOffset = std::copy( ItRecv, m_vecRecvBuf.begin()+ iReceiveOffset + iCount, m_vecRecvBuf.begin() ) - m_vecRecvBuf.begin();
		}else
			iReceiveOffset = 0;
		//Увеличиваем размер буфера при необходимости
		if( (iCount == ( (int)m_vecRecvBuf.size()-iReceiveOffset ) ) )
			if( (m_vecRecvBuf.size()<<1) <= RECEIVE_BUF_MAX_SIZE )
				m_vecRecvBuf.resize( m_vecRecvBuf.size()<<1 );
			else if( m_vecRecvBuf.size() < RECEIVE_BUF_MAX_SIZE )
				m_vecRecvBuf.resize( RECEIVE_BUF_MAX_SIZE );
		Log::instance().Trace( 80, "CAgentHandler::SendMessage: Размер приемного буфера: %d", m_vecRecvBuf.size() );
	}
	if( 0 == iCount )
		throw HandlerErr( "Connection closed" );
	return (enumAgentResponse)Result;
}

void CAgentHandler::Open()
{
	if( m_Sock.IsConnected() )
		return;
	try
	{
		int iAgentListenPort;
		Settings::instance().GetParam( AGENT_LISTEN_PORT, iAgentListenPort );
		Log::instance().Trace( 90, "CAgentHandler::Open: открытие" );
		m_Sock.Connect( m_strAddress, iAgentListenPort );
	}catch( SocketErr& e )
	{
		Log::instance().Trace( 50, "CAgentHandler::Open: Ошибка соединения с агентом: %s; Описание ошибки: %s", m_strAddress.c_str(), e.what() );
	}
}

void CAgentHandler::Close()
{
	try
	{
		Log::instance().Trace( 90, "CAgentHandler::Close: закрытие" );
		if( !m_Sock.IsConnected() )
			return;
		m_Sock.Close();
	}catch( SocketErr& e )
	{
		Log::instance().Trace( 50, "CAgentHandler::Close: ошибка закрытия соединения %s", e.what() );
	}
}

bool CAgentHandler::IsOpened()const
{
	return m_Sock.IsConnected();
}

void CAgentHandler::OnConnection( SmartPtr< CSocket > pSocket )
{
	Log::instance().Trace( 90, "CAgentHandler::OnConnection:.." );
	m_pConnectionHandler->Listen( pSocket );
}

void CAgentHandler::OnMessage( CPacket& Msg )
{
	try{
		BYTE bCommandId;
		Msg.GetCommandId( bCommandId );
		switch( bCommandId )
		{
			case ScanComplete:
				Log::instance().Trace( 90, "CAgentHandler::OnMessage: Сканирование закончено" );
				GetData();
				break;
		};
	}catch(std::exception& e)
	{
		Log::instance().Trace( 10, "CAgentHandler::OnMessage: Ошибка %s", e.what() );
	}
	catch(...)
	{
		Log::instance().Trace( 10, "CAgentHandler::OnMessage: Неизвестная ошибка " );
	}

}

enumAgentResponse CAgentHandler::BeginScan( std::vector< std::string > vecAddresses )
{
	Log::instance().Trace( 90, "CAgentHandler::BeginScan: Отправка команды начала сканирования" );
	CPacket Msg;

	Msg.BeginCommand( START_SCAN );
	Log::instance().Trace( 90, "CAgentHandler::BeginScan: Всего адресов: %d", vecAddresses.size() );
	Msg.AddParam( (DWORD)vecAddresses.size() );
	for( std::vector< std::string >::iterator It = vecAddresses.begin(); It != vecAddresses.end(); It++ )
	{
		Log::instance().Trace( 92, "CAgentHandler::BeginScan: Добавляем адрес %s", It->c_str() );
		Msg.AddAddress( *It );
	}
	Msg.EndCommand();
	
	Log::instance().Trace( 92, "CAgentHandler::BeginScan: Отправляем сообщение агенту: %s", m_strAddress.c_str() );
	std::vector<BYTE> vecRes;
	CBufReceiver Receiver( vecRes );
	enumAgentResponse res = SendMessage( Msg, Receiver );
	m_ScanFinished.Reset();
	return res; 
}
	
enumAgentResponse CAgentHandler::StopScan()
{
	Log::instance().Trace( 90, "CAgentHandler::StopScan: Отправка команды окончания сканирования" );
	CPacket Msg;

	Msg.BeginCommand( STOP_SCAN );
	Msg.EndCommand();

	SmartPtr< BYTE, AllocMalloc<BYTE> >  pbRecvBuf;
	std::vector<BYTE> vecRes;
	CBufReceiver Receiver( vecRes );
	enumAgentResponse res = SendMessage( Msg, Receiver );
	return res; 
}
	
enumAgentResponse CAgentHandler::GetStatus( enumAgentState& Status )
{
	Log::instance().Trace( 90, "CAgentHandler::GetStatus: Отправка команды получения статуса" );
	CPacket Msg;

	Msg.BeginCommand( GET_STATUS );
	Msg.EndCommand();

	std::vector<BYTE> vecRes;
	CBufReceiver Receiver( vecRes );
	enumAgentResponse res = SendMessage( Msg, Receiver );
	if( RESP_OK != res )
	{
		Log::instance().Trace( 50, "CAgentHandler::GetStatus: Команда получения статуса не выполнена, код возврата: %d", res );
		return res;
	}
	Status = (enumAgentState)vecRes[0];
	Log::instance().Trace( 80, "CAgentHandler::GetStatus: Получен статус: %d", Status );
	return RESP_OK;
}

static DWORD dwTime = 0;
	
CDbReceiver::buf_t::iterator CDbReceiver::AddData( buf_t::iterator begin, buf_t::iterator end )
{
	
	//Текущее смещение в пакете
	int iOffset = 0;
	//Размер данных приходит один раз в первом пакете
	//TODO:нигде не используется
	if( 0 == m_ulDataSize )
	{
		::memcpy( (BYTE*)&m_ulDataSize, &(*begin), sizeof( unsigned long ) );
		Log::instance().Trace( 80, "CDbReceiver::AddData: Размер данных: %d", m_ulDataSize );
		iOffset = 4;
	}
	//Разбираем данные

	//Промежуточный буфер,нужен для того чтобы записывать файлы в базу не по одному,что очень долго
	//а пакетами
	map< std::string, std::list<fileStr> > mapTmpBuf;
	unsigned long ulTmpBufSize = 0;
	while( iOffset < (int)( end - begin )  )
	{
		fileStr file;
		::ZeroMemory( &file.FDate, sizeof( fileDate ) );
		//Если запись неполная - заканчиваем разбор
		if( end == std::find( begin + iOffset, end, 0 ) )
			break;
        std::string strIpAddr = (char*)&*(begin + iOffset);
		iOffset += (int)strIpAddr.size() + 1;
		//Если запись неполная - заканчиваем разбор
		if( end == std::find( begin + iOffset, end, 0 ) )
		{
			iOffset -= ( (int)strIpAddr.size() + 1 );
			break;
		}
		file.FileName = (char*)&*(begin + iOffset);
		iOffset += file.FileName.size() + 1;
		//Если запись неполная - заканчиваем разбор
		if( ( end - ( begin + iOffset + sizeof( __int64 ) + 2*sizeof( DWORD ) ) ) < 0 )
		{
			iOffset -= ( (int)strIpAddr.size() + 1 + file.FileName.size() + 1 );
			break;
		}

		memcpy( &file.FileSize, &*(begin + iOffset), sizeof( __int64 ) );
		iOffset += sizeof( __int64 );
		memcpy( &file.FDate.lFileTime, &*(begin + iOffset), sizeof( DWORD ) );
		iOffset += sizeof( DWORD );
		memcpy( &file.FDate.hFileTime, &*(begin + iOffset), sizeof( DWORD ) );
		iOffset += sizeof( DWORD );
		Log::instance().Trace( 101, "Добавляем запись: IP=%s,FileName=%s,\r\n FileSize=%d", strIpAddr.c_str(), file.FileName.c_str(), file.FileSize );
		mapTmpBuf[ strIpAddr ].push_back( file );
		ulTmpBufSize += file.FileName.capacity() + sizeof( __int64 ) + 2*sizeof(DWORD);
		if( mapTmpBuf.end() == mapTmpBuf.find( strIpAddr ) )
			ulTmpBufSize += strIpAddr.capacity();

		//Если размер промежуточного буфера превысил 2Мб - скидываем его в базу
		if( ulTmpBufSize > 2097152 )
		{
			Log::instance().Trace( 90, "CDbReceiver::AddData: Записываем данные в базу" );
			ulTmpBufSize = 0;
			for( std::map< std::string, std::list< fileStr > >::iterator It = mapTmpBuf.begin(); It != mapTmpBuf.end(); It++ )
			{
				hostRec rec;
				rec.IPNum = It->first;
				rec.Files.swap( It->second );
				if( m_mapErased.find( rec.IPNum ) == m_mapErased.end() )
				{
					DbProviderFactory::instance().GetProviderInstance()->EraseHost( "", rec.IPNum, 0 );
					m_mapErased.insert( rec.IPNum );
				}
				DWORD dwtick1,dwtick2;
				dwtick1 = GetTickCount();
				DbProviderFactory::instance().GetProviderInstance()->AddFiles( rec );
				dwtick2 = GetTickCount();
				dwTime += dwtick2 - dwtick1;

			}
			mapTmpBuf.clear();
		}
		
	}
	//Дописываем в базу остатки
	Log::instance().Trace( 90, "CDbReceiver::AddData: Записываем данные в базу" );
	for( std::map< std::string, std::list< fileStr > >::iterator It = mapTmpBuf.begin(); It != mapTmpBuf.end(); It++ )
	{
		hostRec rec;
		rec.IPNum = It->first;
		rec.Files.swap( It->second );
		if( m_mapErased.find( rec.IPNum ) == m_mapErased.end() )
		{
			DbProviderFactory::instance().GetProviderInstance()->EraseHost( "", rec.IPNum, 0 );
			m_mapErased.insert( rec.IPNum );
		}
		DWORD dwtick1,dwtick2;
		dwtick1 = GetTickCount();
		DbProviderFactory::instance().GetProviderInstance()->AddFiles( rec );

		dwtick2 = GetTickCount();
		dwTime += dwtick2 - dwtick1;
	}
	mapTmpBuf.clear();
	if( iOffset > (int)(end - begin) )
	{
		Log::instance().Trace( 2, "CDbReceiver::AddData: Случилось чтото странное, iOffset > (end-begin)!!!Требуется исследование ситуации." );
		return end;
	}	
	return (begin + iOffset);
}

enumAgentResponse CAgentHandler::GetData()
{
	Log::instance().Trace( 90, "CAgentHandler::GetData: Отправка команды получения данных" );	
	CPacket Msg;

	Msg.BeginCommand( GET_DATA );
	Msg.EndCommand();

	//отправляем команду
	std::vector<BYTE> vecRes;
	CDbReceiver Receiver;
	enumAgentResponse res = SendMessage( Msg, Receiver );
	Log::instance().Trace( 0, "Суммарное время записи в БД: %d", dwTime );
	m_ScanFinished.Set();
	return res;
}

void CConnectionHandler::Listen( SmartPtr<CSocket> pSocket )
{
	m_pSocket = pSocket;
	if( WAIT_OBJECT_0 != WaitForSingleObject( m_hListenThread, 0 ) )
		m_hListenThread = (HANDLE)_beginthreadex( 0, 0, fnListenThread, this, 0, NULL );
}

unsigned __stdcall CConnectionHandler::fnListenThread( void* param )
{
	Log::instance().Trace( 90, "CConnectionHandler::fnListenThread: Запуск" );
	CConnectionHandler* pThis = (CConnectionHandler*)param;
	try{
		CPacket Msg;
		BYTE pBuf[10240];
		int iCount;
		while( iCount = pThis->m_pSocket->Receive( pBuf, sizeof( pBuf ) ) )
		{
			Log::instance().Dump( 90, pBuf, iCount, "CConnectionHandler::fnListenThread: Получен пакет:" );
			Msg.SetBuffer( pBuf, iCount );
			pThis->m_pAgentHandler->OnMessage( Msg );
		}
	}catch( std::exception& e )
	{
		Log::instance().Trace( 10," CConnectionHandler::fnListenThread: Возникло исключение: %s", e.what() );
	}catch( ... )
	{
		Log::instance().Trace( 10," CConnectionHandler::fnListenThread: Возникло неизвестное исключение" );
	}
	Log::instance().Trace( 90, "CConnectionHandler::fnListenThread: Закрытие" );
	CloseHandle( pThis->m_hListenThread );
	return 0;
}

CConnectionHandler::~CConnectionHandler()
{
	//Если соединение событий существует - закрываем его
	if( NULL != m_pSocket.get() )
		m_pSocket->Close();
	SetEvent( m_hCloseEv );
	WaitForSingleObject( m_hListenThread, 10000 );
	CloseHandle( m_hCloseEv );
}
