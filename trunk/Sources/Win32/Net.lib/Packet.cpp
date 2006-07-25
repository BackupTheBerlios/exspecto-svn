//-------------------------------------------------------------------------------------
//Этот файл является частью проекта Exspecto 2006г.
//Module: CPacket class
//Author: Parshin Dmitry
//Description: Класс, реализующий функции для работы с пакетами (парсинг и подготовка)
//-------------------------------------------------------------------------------------
#include "StdAfx.h"
#include ".\packet.h"


CPacket::CPacket(void):m_iDataSize( 0 )
					  ,m_iBufSize( 1024 )
					  ,m_iOffset( 0 )
{
	//Выделяем память под буфер,предварительно 1Кб 
	m_pbBuf = (BYTE*)malloc( 1024 );
}

CPacket::~CPacket(void)
{
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
		m_pbBuf = (BYTE*)realloc( m_pbBuf, m_iBufSize + 1024 );

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
	//TODO:
	//возможна ошибка,переставить местами слова
	Push( (BYTE*)&dwParam, sizeof(DWORD) );
}

//Добавить параметр
//	strParam - строковый параметр
void CPacket::AddParam( std::string strParam )
{
	Push( (BYTE*)strParam.c_str(), (int)strParam.size() );
}

//Добавить IP - адрес в пакет
bool CPacket::AddAddress( std::string strAddress )
{
	u_long lAdr;
	//Передаем адрес в сетевом формате - 4 байта
	if( INADDR_NONE ==( lAdr = inet_addr( strAddress.c_str() ) ) )
		return false;
	else
	{
        Push( (BYTE*)&lAdr, sizeof( u_long ) );
	}
	return true;
}

//Получить массив байт из пакета по текущему смещению
bool CPacket::GetParam( BYTE* pbValue, int iSize )
{
	if( iSize > m_iDataSize - m_iOffset )
		return false;

	Pop( pbValue, iSize );
	return true;
}

//Получить параметр типа DWORD по текущему смещению
bool CPacket::GetParam( DWORD& dwValue )
{
	if( sizeof(DWORD) > m_iDataSize - m_iOffset )
		return false;

	Pop( (BYTE*)&dwValue, sizeof(DWORD) );
	return true;
}

//Получить строку длиной iSize из пакета по текущему смещению
bool CPacket::GetParam( std::string& strValue, int iSize )
{
	BYTE* strTmp = new BYTE[ iSize + 1 ];
	strTmp[ iSize ] = 0; //string zero
	if( iSize > m_iDataSize - m_iOffset )
		return false;

	Pop( strTmp, iSize );
	strValue = (char*)strTmp;
	delete strTmp;
	return true;
}

//Получить IP-адрес из пакета по текущему смещению
bool CPacket::GetAddress( std::string& strAddress )
{
	in_addr ulAdr;
	char* strAdr;
	if( sizeof( u_long ) > m_iDataSize - m_iOffset )
		return false;

	Pop( (BYTE*)&ulAdr.S_un.S_addr, sizeof( u_long ) );

	//Переводим адрес из сетевого формата в строку
	if( NULL == ( strAdr = ::inet_ntoa( ulAdr ) ) )
		return false;

	strAddress = strAdr;
	return true;
}

//Добавить метку конца пакета
void CPacket::EndCommand()
{
	Push( (BYTE*)"END", 4 );
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
	{
		m_pbBuf = (BYTE*)realloc( m_pbBuf, iSize );
	}
	::memcpy( m_pbBuf ,pbBuffer, iSize );
	m_iDataSize = iSize;
	m_iOffset = 0;
}

//Получить идентификатор команды по текущему смещению в пакете
bool CPacket::GetCommandId( BYTE& pByte )
{
	if( m_iOffset == m_iDataSize )
		return false;
	Pop( &pByte, 1 );
	return true;
}

//Получить массив байт из пакета
void CPacket::Pop( BYTE *pbBuf, int iCount )
{
	::memcpy( pbBuf, m_pbBuf + m_iOffset, iCount );
	m_iOffset += iCount;
	if( 'E' == m_pbBuf[ m_iOffset ] )
		m_iOffset += 3;
}

//Отправить пакет в сокет
//Синтаксис: CSocket sock;CPacket packet; sock << packet;
CPacket& operator <<( CSocket& sock, CPacket& packet )
{
	BYTE* Buf = NULL;
	int iSize;
	packet.GetBuffer( Buf, iSize );
    sock.Send( Buf, iSize );
	return packet;
}