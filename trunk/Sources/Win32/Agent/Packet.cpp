#include "StdAfx.h"
#include ".\packet.h"


CPacket::CPacket(void):m_iDataSize( 0 )
					  ,m_iBufSize( 1024 )
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

void CPacket::AddBinParam( BYTE* pbParam, int iSize )
{
	Push( pbParam, iSize );
}

void CPacket::AddDwordParam( DWORD dwParam )
{
	//TODO:
	//возможна ошибка,переставить местами слова
	Push( (BYTE*)&dwParam, 4 );
}

void CPacket::AddStrParam( std::string strParam )
{
	Push( (BYTE*)strParam.c_str(), (int)strParam.size() );
}

void CPacket::EndCommand()
{
	Push( '\0', 1 );
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
	m_pbBuf = pbBuffer;
	m_iDataSize = iSize;
}

CPacket& operator <<( CSocket& sock, CPacket& packet )
{
	BYTE* Buf = NULL;
	int iSize;
	packet.GetBuffer( Buf, iSize );
    sock.Send( Buf, iSize );
	return packet;
}