//-------------------------------------------------------------------------------------
//Этот файл является частью проекта Exspecto 2006г.
//Module: CPacket class
//Author: Parshin Dmitry
//Description: Класс, реализующий функции для работы с пакетами (парсинг и подготовка)
//-------------------------------------------------------------------------------------
#include "packet.h"
#include "precomp.h"


CPacket::CPacket(void)
{
}

CPacket::~CPacket(void)
{
}

//Добавить в пакет идентификатор команды
void CPacket::BeginCommand( enumCommands Command )
{
	Push( (BYTE*)&Command, 1 );
}

//Добавить массив байт к пакету
void CPacket::Push( const BYTE* pbData, int iSize )
{
	m_vecBuf.insert( m_vecBuf.end(), pbData, pbData + iSize );
}

//Добавить параметр
//	pbParam - буфер с данными
//	iSize - размер данных
void CPacket::AddParam( const BYTE* pbParam, int iSize )
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
void CPacket::AddParam( const std::string& strParam )
{
	Push( (BYTE*)strParam.c_str(), (int)strParam.size() );
}

//Добавить IP - адрес в пакет
void CPacket::AddAddress( const std::string& strAddress )
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
	if( iSize > (int)m_vecBuf.size() - 3 )
		throw PacketFormatErr( "Incorrect requested param size" );
	Pop( pbValue, iSize );
}

//Получить параметр типа DWORD по текущему смещению
void CPacket::GetParam( DWORD& dwValue )
{
	if( sizeof(DWORD) > m_vecBuf.size() - 3 )
		throw PacketFormatErr( "Incorrect param size" );

	Pop( (BYTE*)&dwValue, sizeof(DWORD) );
}

//Получить строку длиной iSize из пакета по текущему смещению
void CPacket::GetParam( std::string& strValue, int iSize )
{
	BYTE* strTmp = new BYTE[ iSize + 1 ];
	strTmp[ iSize ] = 0; //string zero
	if( iSize > (int)m_vecBuf.size() - 3 )
		throw PacketFormatErr( "Incorrect requested param size" );

	Pop( strTmp, iSize );
	strValue = (char*)strTmp;
	delete strTmp;
}

//Получить IP-адрес из пакета по текущему смещению
void CPacket::GetAddress( std::string& strAddress )
{
	in_addr ulAdr;
	char* strAdr;
	if( sizeof( u_long ) > m_vecBuf.size() - 3 )
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
	m_vecBuf.clear();
}

//Получить адрес буфера для отправки пакета(адрес данных подготовленного пакета)
void CPacket::GetBuffer( OUT BYTE* &pbBuffer, OUT int &iSize )
{
	pbBuffer = &m_vecBuf[0];
	iSize = (int)m_vecBuf.size();
}

//Установить данные для разбора
void CPacket::SetBuffer( IN BYTE* pbBuffer, IN int iSize )
{
	m_vecBuf.insert( m_vecBuf.begin(), pbBuffer, pbBuffer + iSize );
	m_vecBuf.resize( iSize );
}

//Получить идентификатор команды по текущему смещению в пакете
void CPacket::GetCommandId( BYTE& pByte )
{
	if( m_vecBuf.empty() )
		throw PacketFormatErr( "Incorrect packet format" );
	Pop( &pByte, 1 );
}

//Получить массив байт из пакета
void CPacket::Pop( BYTE *pbBuf, int iCount )
{
	::memcpy( pbBuf, &m_vecBuf[0], iCount );
	m_vecBuf.erase( m_vecBuf.begin(), m_vecBuf.begin() + iCount );
	if( 0 == ::memcmp( &m_vecBuf[0], "END", 3 ) )
	{
		m_vecBuf.erase( m_vecBuf.begin(), m_vecBuf.begin() + 3 );
		if( !m_vecBuf.empty() )
			throw PacketFormatErr( "Incorrect packet format" );
	}
}

//Выяснить закончена обработка пакета или нет
bool CPacket::IsDone()
{
	return m_vecBuf.empty();
}
