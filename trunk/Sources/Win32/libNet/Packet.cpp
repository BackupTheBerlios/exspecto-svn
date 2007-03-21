//-------------------------------------------------------------------------------------
//���� ���� �������� ������ ������� Exspecto 2006�.
//Module: CPacket class
//Author: Parshin Dmitry
//Description: �����, ����������� ������� ��� ������ � �������� (������� � ����������)
//-------------------------------------------------------------------------------------
#include "packet.h"
#include "precomp.h"


CPacket::CPacket(void):m_iDataSize( 0 )
					  ,m_iBufSize( 1024 )
					  ,m_iOffset( 0 )
{
	//�������� ������ ��� �����,�������������� 1�� 
	if( NULL == ( m_pbBuf = (BYTE*)malloc( 1024 ) ) )
		throw PacketErr( "Not enough memory to form the packet" );
}

CPacket::~CPacket(void)
{
	if( NULL != m_pbBuf )
		free( m_pbBuf );
}

//�������� � ����� ������������� �������
void CPacket::BeginCommand( enumCommands Command )
{
	Push( (BYTE*)&Command, 1 );
}

//�������� ������ ���� � ������
void CPacket::Push( BYTE* pbData, int iSize )
{
	m_iDataSize += iSize;
	if( m_iDataSize > m_iBufSize )
		if( NULL == ( m_pbBuf = (BYTE*)realloc( m_pbBuf, iSize + 1024 ) ) )
			throw PacketErr( "Not enough memory to form the packet" );
	::memcpy( m_pbBuf + m_iDataSize - iSize, pbData, iSize );
}

//�������� ��������
//	pbParam - ����� � �������
//	iSize - ������ ������
void CPacket::AddParam( BYTE* pbParam, int iSize )
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
void CPacket::AddParam( std::string strParam )
{
	Push( (BYTE*)strParam.c_str(), (int)strParam.size() );
}

//�������� IP - ����� � �����
void CPacket::AddAddress( std::string strAddress )
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
	if( iSize > m_iDataSize - m_iOffset )
		throw PacketFormatErr( "Incorrect param size" );
	Pop( pbValue, iSize );
}

//�������� �������� ���� DWORD �� �������� ��������
void CPacket::GetParam( DWORD& dwValue )
{
	if( sizeof(DWORD) > ( unsigned int )( m_iDataSize - m_iOffset ) )
		throw PacketFormatErr( "Incorrect param size" );

	Pop( (BYTE*)&dwValue, sizeof(DWORD) );
}

//�������� ������ ������ iSize �� ������ �� �������� ��������
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

//�������� IP-����� �� ������ �� �������� ��������
void CPacket::GetAddress( std::string& strAddress )
{
	in_addr ulAdr;
	char* strAdr;
	if( sizeof( u_long ) > ( unsigned int ) ( m_iDataSize - m_iOffset ) )
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
	::ZeroMemory( m_pbBuf, m_iDataSize );
	m_iDataSize = 0;
}

//�������� ����� ������ ��� �������� ������(����� ������ ��������������� ������)
void CPacket::GetBuffer( OUT BYTE* &pbBuffer, OUT int &iSize )
{
	pbBuffer = m_pbBuf;
	iSize = m_iDataSize;
}

//���������� ������ ��� �������
void CPacket::SetBuffer( IN BYTE* pbBuffer, IN int iSize )
{
	if( m_iBufSize < iSize )
		if( NULL == ( m_pbBuf = (BYTE*)realloc( m_pbBuf, iSize ) ) )
			throw PacketErr( "Not enough memory to form the packet" );
	::memcpy( m_pbBuf ,pbBuffer, iSize );
	m_iDataSize = iSize;
	m_iOffset = 0;
}

//�������� ������������� ������� �� �������� �������� � ������
void CPacket::GetCommandId( BYTE& pByte )
{
	if( m_iOffset >= m_iDataSize )
		throw PacketFormatErr( "Incorrect packet format" );
	Pop( &pByte, 1 );
}

//�������� ������ ���� �� ������
void CPacket::Pop( BYTE *pbBuf, int iCount )
{
	::memcpy( pbBuf, m_pbBuf + m_iOffset, iCount );
	m_iOffset += iCount;
	if( !::memcmp( m_pbBuf + m_iOffset, "END", 3 ) )
		m_iOffset += 3;
}

//�������� ��������� ��������� ������ ��� ���
bool CPacket::IsDone()
{
	return m_iOffset >= m_iDataSize;
}

//��������� ����� � �����
//���������: CSocket sock;CPacket packet; sock << packet;
CPacket& operator <<( CSocket& sock, CPacket& packet )
{
	//TODO:
	BYTE* Buf = NULL;
	int iSize;
	packet.GetBuffer( Buf, iSize );
    sock.Send( Buf, iSize );
	return packet;
}
