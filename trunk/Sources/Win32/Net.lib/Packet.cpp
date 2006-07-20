#include "StdAfx.h"
#include ".\packet.h"


CPacket::CPacket(void):m_iDataSize( 0 )
					  ,m_iBufSize( 1024 )
					  ,m_iOffset( 0 )
{
	m_pbBuf = (BYTE*)malloc( 1024 );
}

CPacket::~CPacket(void)
{
	free( m_pbBuf );
}

void CPacket::BeginCommand( enumCommands Command )
{
	Push( (BYTE*)&Command, 1 );
}

void CPacket::Push( BYTE* pbData, int iSize )
{
	m_iDataSize += iSize;

	if( m_iDataSize > m_iBufSize )
		m_pbBuf = (BYTE*)realloc( m_pbBuf, m_iBufSize + 1024 );

	::memcpy( m_pbBuf + m_iDataSize - iSize, pbData, iSize );
}

void CPacket::AddParam( BYTE* pbParam, int iSize )
{
	Push( pbParam, iSize );
}

void CPacket::AddParam( DWORD dwParam )
{
	//TODO:
	//возможна ошибка,переставить местами слова
	Push( (BYTE*)&dwParam, 4 );
}

void CPacket::AddParam( std::string strParam )
{
	Push( (BYTE*)strParam.c_str(), (int)strParam.size() );
}

bool CPacket::GetParam( BYTE* pbValue, int iSize )
{
	if( iSize > m_iDataSize - m_iOffset )
		return false;

	Pop( pbValue, iSize );
	return true;
}

bool CPacket::GetParam( DWORD& dwValue )
{
	if( sizeof(DWORD) > m_iDataSize - m_iOffset )
		return false;

	Pop( (BYTE*)&dwValue, sizeof(DWORD) );
	return true;
}

bool CPacket::GetParam( std::string& strValue, int iSize )
{
	BYTE* strTmp = new BYTE[ iSize ];
	if( iSize > m_iDataSize - m_iOffset )
		return false;

	Pop( strTmp, iSize );
	strValue = (char*)strTmp;
	delete strTmp;
	return true;
}

void CPacket::EndCommand()
{
	Push( (BYTE*)"END", 4 );
}

void CPacket::Clear()
{
	::ZeroMemory( m_pbBuf, m_iDataSize );
	m_iDataSize = 0;
}

void CPacket::GetBuffer( OUT BYTE* pbBuffer, OUT int &iSize )
{
	pbBuffer = m_pbBuf;
	iSize = m_iDataSize;
}

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

bool CPacket::GetCommandId( BYTE& pByte )
{
	if( m_iOffset == m_iDataSize )
		return false;
	BYTE Buf;
	Pop( &Buf, 1 );
	return true;
}

void CPacket::Pop( BYTE *pbBuf, int iCount )
{
	::memcpy( pbBuf, m_pbBuf, iCount );
	m_iOffset += iCount;
	if( 'E' == m_pbBuf[ m_iOffset ] )
		m_iOffset += 3;
}

CPacket& operator <<( CSocket& sock, CPacket& packet )
{
	BYTE* Buf = NULL;
	int iSize;
	packet.GetBuffer( Buf, iSize );
    sock.Send( Buf, iSize );
	return packet;
}