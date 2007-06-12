//-------------------------------------------------------------------------------------
//���� ���� �������� ������ ������� Exspecto 2006�.
//Module: CPacket class
//Author: Parshin Dmitry
//Description: �����, ����������� ������� ��� ������ � �������� (������� � ����������)
//-------------------------------------------------------------------------------------
#include "packet.h"
#include "precomp.h"


CPacket::CPacket(void)
{
}

CPacket::~CPacket(void)
{
}

//�������� � ����� ������������� �������
void CPacket::BeginCommand( enumCommands Command )
{
	Push( (BYTE*)&Command, 1 );
}

//�������� ������ ���� � ������
void CPacket::Push( const BYTE* pbData, int iSize )
{
	m_vecBuf.insert( m_vecBuf.end(), pbData, pbData + iSize );
}

//�������� ��������
//	pbParam - ����� � �������
//	iSize - ������ ������
void CPacket::AddParam( const BYTE* pbParam, int iSize )
{
	Push( pbParam, iSize );
}

//�������� ��������
//	dwParam - �������� ���� DWORD
void CPacket::AddParam( DWORD dwParam )
{
	Push( (BYTE*)&dwParam, sizeof(DWORD) );
}

//�������� ��������
//	strParam - ��������� ��������
void CPacket::AddParam( const std::string& strParam )
{
	Push( (BYTE*)strParam.c_str(), (int)strParam.size() );
}

//�������� IP - ����� � �����
void CPacket::AddAddress( const std::string& strAddress )
{
	u_long lAdr;
	//�������� ����� � ������� ������� - 4 �����
	if( INADDR_NONE ==( lAdr = inet_addr( strAddress.c_str() ) ) )
		throw PacketFormatErr( "Address is incorrect" );
    Push( (BYTE*)&lAdr, sizeof( u_long ) );
}

//�������� ������ ���� �� ������ �� �������� ��������
void CPacket::GetParam( BYTE* pbValue, int iSize )
{
	if( iSize > (int)m_vecBuf.size() - 3 )
		throw PacketFormatErr( "Incorrect requested param size" );
	Pop( pbValue, iSize );
}

//�������� �������� ���� DWORD �� �������� ��������
void CPacket::GetParam( DWORD& dwValue )
{
	if( sizeof(DWORD) > m_vecBuf.size() - 3 )
		throw PacketFormatErr( "Incorrect param size" );

	Pop( (BYTE*)&dwValue, sizeof(DWORD) );
}

//�������� ������ ������ iSize �� ������ �� �������� ��������
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

//�������� IP-����� �� ������ �� �������� ��������
void CPacket::GetAddress( std::string& strAddress )
{
	in_addr ulAdr;
	char* strAdr;
	if( sizeof( u_long ) > m_vecBuf.size() - 3 )
		throw PacketFormatErr( "Incorrect param size" );

	Pop( (BYTE*)&ulAdr.S_un.S_addr, sizeof( u_long ) );

	//��������� ����� �� �������� ������� � ������
	if( NULL == ( strAdr = ::inet_ntoa( ulAdr ) ) )
		throw PacketFormatErr( "Address is incorrect" );

	strAddress = strAdr;
}

//�������� ����� ����� ������
void CPacket::EndCommand()
{
	Push( (BYTE*)"END", 3 );
}

//�������� �����
void CPacket::Clear()
{
	m_vecBuf.clear();
}

//�������� ����� ������ ��� �������� ������(����� ������ ��������������� ������)
void CPacket::GetBuffer( OUT BYTE* &pbBuffer, OUT int &iSize )
{
	pbBuffer = &m_vecBuf[0];
	iSize = (int)m_vecBuf.size();
}

//���������� ������ ��� �������
void CPacket::SetBuffer( IN BYTE* pbBuffer, IN int iSize )
{
	m_vecBuf.insert( m_vecBuf.begin(), pbBuffer, pbBuffer + iSize );
	m_vecBuf.resize( iSize );
}

//�������� ������������� ������� �� �������� �������� � ������
void CPacket::GetCommandId( BYTE& pByte )
{
	if( m_vecBuf.empty() )
		throw PacketFormatErr( "Incorrect packet format" );
	Pop( &pByte, 1 );
}

//�������� ������ ���� �� ������
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

//�������� ��������� ��������� ������ ��� ���
bool CPacket::IsDone()
{
	return m_vecBuf.empty();
}
