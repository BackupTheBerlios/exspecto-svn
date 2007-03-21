//-------------------------------------------------------------------------------------
//Этот файл является частью проекта Exspecto 2006г.
//Module: CPacket class
//Author: Parshin Dmitry
//Description: Класс, реализующий функции для работы с пакетами (парсинг и подготовка)
//-------------------------------------------------------------------------------------
#include "packet.h"
#include "precomp.h"


CPacket::CPacket(void):m_iDataSize( 0 )
					  ,m_iBufSize( 1024 )
					  ,m_iOffset( 0 )
{
	//Выделяем память под буфер,предварительно 1Кб 
	if( NULL == ( m_pbBuf = (BYTE*)malloc( 1024 ) ) )
		throw PacketErr( "Not enough memory to form the packet" );
}

CPacket::~CPacket(void)
{
	if( NULL != m_pbBuf )
		free( m_pbBuf );
}

//Добавить в пакет идентификатор команды
void CPacket::BeginCommand( enumCommands Command )
{
	Push( (BYTE*)&Command, 1 );
}

//Добавить массив байт к пакету
void CPacket::Push( BYTE* pbData, int iSize )
{
	m_iDataSize += iSize;
	if( m_iDataSize > m_iBufSize )
		if( NULL == ( m_pbBuf = (BYTE*)realloc( m_pbBuf, iSize + 1024 ) ) )
			throw PacketErr( "Not enough memory to form the packet" );
	::memcpy( m_pbBuf + m_iDataSize - iSize, pbData, iSize );
}

//Добавить параметр
//	pbParam - буфер с данными
//	iSize - размер данных
void CPacket::AddParam( BYTE* pbParam, int iSize )
{
	Push( pbParam, iSize );
}

//Добавить параметр
//	dwParam - параметр типа DWORD
void CPacket::AddParam( DWORD dwParam )
{
	Push( (BYTE*)&dwParam, sizeof(DWORD) );
}

//Добавить параметр
//	strParam - строковый параметр
void CPacket::AddParam( std::string strParam )
{
	Push( (BYTE*)strParam.c_str(), (int)strParam.size() );
}

//Добавить IP - адрес в пакет
void CPacket::AddAddress( std::string strAddress )
{
	u_long lAdr;
	//Передаем адрес в сетевом формате - 4 байта
	if( INADDR_NONE ==( lAdr = inet_addr( strAddress.c_str() ) ) )
		throw PacketFormatErr( "Address is incorrect" );
    Push( (BYTE*)&lAdr, sizeof( u_long ) );
}

//Получить массив байт из пакета по текущему смещению
void CPacket::GetParam( BYTE* pbValue, int iSize )
{
	if( iSize > m_iDataSize - m_iOffset )
		throw PacketFormatErr( "Incorrect param size" );
	Pop( pbValue, iSize );
}

//Получить параметр типа DWORD по текущему смещению
void CPacket::GetParam( DWORD& dwValue )
{
	if( sizeof(DWORD) > ( unsigned int )( m_iDataSize - m_iOffset ) )
		throw PacketFormatErr( "Incorrect param size" );

	Pop( (BYTE*)&dwValue, sizeof(DWORD) );
}

//Получить строку длиной iSize из пакета по текущему смещению
void CPacket::GetParam( std::string& strValue, int iSize )
{
	BYTE* strTmp = new BYTE[ iSize + 1 ];
	strTmp[ iSize ] = 0; //string zero
	if( iSize > m_iDataSize - m_iOffset )
		throw PacketFormatErr( "Incorrect param size" );

	Pop( strTmp, iSize );
	strValue = (char*)strTmp;
	delete strTmp;
}

//Получить IP-адрес из пакета по текущему смещению
void CPacket::GetAddress( std::string& strAddress )
{
	in_addr ulAdr;
	char* strAdr;
	if( sizeof( u_long ) > ( unsigned int ) ( m_iDataSize - m_iOffset ) )
		throw PacketFormatErr( "Incorrect param size" );

	Pop( (BYTE*)&ulAdr.S_un.S_addr, sizeof( u_long ) );

	//Переводим адрес из сетевого формата в строку
	if( NULL == ( strAdr = ::inet_ntoa( ulAdr ) ) )
		throw PacketFormatErr( "Address is incorrect" );

	strAddress = strAdr;
}

//Добавить метку конца пакета
void CPacket::EndCommand()
{
	Push( (BYTE*)"END", 3 );
}

//Очистить пакет
void CPacket::Clear()
{
	::ZeroMemory( m_pbBuf, m_iDataSize );
	m_iDataSize = 0;
}

//Получить адрес буфера для отправки пакета(адрес данных подготовленного пакета)
void CPacket::GetBuffer( OUT BYTE* &pbBuffer, OUT int &iSize )
{
	pbBuffer = m_pbBuf;
	iSize = m_iDataSize;
}

//Установить данные для разбора
void CPacket::SetBuffer( IN BYTE* pbBuffer, IN int iSize )
{
	if( m_iBufSize < iSize )
		if( NULL == ( m_pbBuf = (BYTE*)realloc( m_pbBuf, iSize ) ) )
			throw PacketErr( "Not enough memory to form the packet" );
	::memcpy( m_pbBuf ,pbBuffer, iSize );
	m_iDataSize = iSize;
	m_iOffset = 0;
}

//Получить идентификатор команды по текущему смещению в пакете
void CPacket::GetCommandId( BYTE& pByte )
{
	if( m_iOffset >= m_iDataSize )
		throw PacketFormatErr( "Incorrect packet format" );
	Pop( &pByte, 1 );
}

//Получить массив байт из пакета
void CPacket::Pop( BYTE *pbBuf, int iCount )
{
	::memcpy( pbBuf, m_pbBuf + m_iOffset, iCount );
	m_iOffset += iCount;
	if( !::memcmp( m_pbBuf + m_iOffset, "END", 3 ) )
		m_iOffset += 3;
}

//Выяснить закончена обработка пакета или нет
bool CPacket::IsDone()
{
	return m_iOffset >= m_iDataSize;
}

//Отправить пакет в сокет
//Синтаксис: CSocket sock;CPacket packet; sock << packet;
CPacket& operator <<( CSocket& sock, CPacket& packet )
{
	//TODO:
	BYTE* Buf = NULL;
	int iSize;
	packet.GetBuffer( Buf, iSize );
    sock.Send( Buf, iSize );
	return packet;
}
