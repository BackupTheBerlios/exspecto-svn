//-------------------------------------------------------------------------------------
//���� ���� �������� ������ ������� Exspecto 2006�.
//Module: CPacket class
//Author: Parshin Dmitry
//Description: �����, ����������� ������� ��� ������ � �������� (������� � ����������)
//-------------------------------------------------------------------------------------
#include "packet.h"
#include "precomp.h"
#include <sstream>
#include "constants.h"
#include <time.h>


CInPacket::CInPacket( BYTE* pbBuf, int iSize ):m_pXmlRoot( NULL )
											  ,m_pHostElement( NULL )
											  ,m_pAddrElement( NULL )
{
	std::string strPacket( pbBuf, pbBuf+iSize );
	if( ( NULL == m_XmlDoc.Parse( strPacket.c_str() ) ) || ( NULL == ( m_pXmlRoot = m_XmlDoc.RootElement() ) ) )
	{
		Log::instance().Trace( 1, "CInPacket::CInPacket: �� ������� ��������� xml �����: %s", strPacket.c_str() );
		std::string strErr = "�� ������� ��������� �������� xml-�����: ";
		strErr += m_XmlDoc.ErrorDesc();
		throw PacketErr( strErr );
	}
}

CInPacket::CInPacket():m_pXmlRoot( NULL )
					  ,m_pHostElement( NULL )
					  ,m_pAddrElement( NULL )
{}

bool CInPacket::Load( BYTE* pbBuf, int iSize )
{
	m_XmlDoc.Clear();
	std::string strPacket( pbBuf, pbBuf+iSize );
	if( ( NULL == m_XmlDoc.Parse( strPacket.c_str() ) ) || ( NULL == ( m_pXmlRoot = m_XmlDoc.RootElement() ) ) )
	{
		Log::instance().Trace( 1, "CInPacket::Load: �� ������� ��������� xml �����: %s", strPacket.c_str() );
		std::string strErr = "�� ������� ��������� �������� xml-�����: ";
		strErr += m_XmlDoc.ErrorDesc();
		std::stringstream ss;
		ss << " Row: " << m_XmlDoc.ErrorRow() << " Col: " << m_XmlDoc.ErrorCol();
		throw PacketErr( strErr + ss.str() );
	}
	return true;
}

void CInPacket::GetField( const std::string& strName, std::string& strValue )
{
	GetField( m_pXmlRoot, strName, strValue );
}

void CInPacket::GetField( const std::string& strName, int& iValue )
{
	GetField( m_pXmlRoot, strName, iValue );
}

void CInPacket::GetField( const std::string& strName, bool& bValue )
{
	GetField( m_pXmlRoot, strName, bValue );
}

void CInPacket::GetFirstAddress( std::string& strAddress )
{
	m_pAddrElement = m_pXmlRoot->FirstChildElement( HOST_ADDR );
	if( NULL == m_pAddrElement )
	{
		Log::instance().Trace( 1, "%s: �� ������� ���� %s ���� ��� �� ����� ����������� ���������", __FUNCTION__, HOST_ADDR ); 
		throw PacketErr( "�� ������� ����" );
	}
	strAddress = m_pAddrElement->GetText();
}

bool CInPacket::GetNextAddress( std::string& strAddress )
{
	if( NULL == m_pAddrElement )
	{
		Log::instance().Trace( 1, "%s: �������� ���������� �� ������� �� ���� ������", __FUNCTION__ ); 
		return false;
	}
	if( NULL == ( m_pAddrElement = m_pAddrElement->NextSiblingElement( HOST_ADDR ) ) )
	{
		m_pAddrElement = NULL;
		return false;
	}
	strAddress = m_pAddrElement->GetText();
	return true;
}

void CInPacket::GetFirstHostRec( hostRec& Host )
{
	if( NULL == ( m_pHostElement = m_pXmlRoot->FirstChildElement( HOST_TAG ) ) )
	{
		Log::instance().Trace( 1, "%s: �� ������� ���� %s ���� ��� �� ����� ����������� ���������", __FUNCTION__, HOST_TAG ); 
		throw PacketErr( "�� ������� ����" );
	}
	GetHostRec( Host );
}

void CInPacket::GetHostRec( hostRec& Host )
{
	if( NULL == m_pHostElement->Attribute( HOST_NAME ) || NULL == m_pHostElement->Attribute( HOST_ADDR ) )
	{
		Log::instance().Trace( 1, "%s: �� ������� ���� %s ���� ��� �� ����� ����������� ���������", __FUNCTION__, HOST_TAG ); 
		throw PacketErr( "�� ������� ����" );
	}
	Host.HostName = m_pHostElement->Attribute( HOST_NAME );
	Host.IPNum = m_pHostElement->Attribute( HOST_ADDR );
	TiXmlElement *pFileElement = m_pHostElement->FirstChildElement( FILE_TAG );
	Host.Files.clear();
	fileStr TmpFile;
	while( NULL != pFileElement )
	{
		GetFileStruct( pFileElement, TmpFile );
		Host.Files.push_back( TmpFile );
		pFileElement = pFileElement->NextSiblingElement( FILE_TAG );
	}

}

bool CInPacket::GetNextHostRec( hostRec& Host )
{
	if( NULL == m_pHostElement )
	{
		Log::instance().Trace( 1, "%s: �������� ���������� � ������ �� ���� ������ ", __FUNCTION__ ); 
		return false;
	}
	if( NULL == ( m_pHostElement = m_pHostElement->NextSiblingElement( HOST_TAG ) ) )
	{
		m_pHostElement = NULL;
		return false;
	}
	GetHostRec( Host );
	return true;
}

void CInPacket::GetFileStruct( TiXmlElement* pElem, fileStr& File )
{
	::ZeroMemory( &File.FDate, sizeof( fileDate ) );
	GetField( pElem, FILE_PATH, File.FileName );
	GetField( pElem, FILE_SIZE, File.FileSize );
	GetField( pElem, FILE_DATE, File.FDate.UTS );
}

void CInPacket::GetField( TiXmlElement* pParentElem, const std::string& strName, std::string& strValue )
{
	if( NULL == pParentElem )
	{
		Log::instance().Trace( 1, "%s: ����� ��� ������� �� ��� ��������", __FUNCTION__ ); 
		throw PacketErr( "����� ��� ������� �� ��� ��������" );
	}
	TiXmlElement* pTmp = pParentElem->FirstChildElement( strName );
	if( NULL == pTmp )
	{
		Log::instance().Trace( 1, "%s: �� ������� ���� %s", __FUNCTION__, strName.c_str() ); 
		throw PacketErr( "�� ������� ����" );
	}
	strValue = pTmp->GetText();
}

void CInPacket::GetField( TiXmlElement* pParentElem, const std::string& strName , int& iValue )
{
	std::string strTmp;
	GetField( pParentElem, strName, strTmp );
	if( ( 0 == ( iValue = atoi( strTmp.c_str() ) ) ) && ( "0" != strTmp ) )
	{
		Log::instance().Trace( 1, "%s: ������ �������������� ���� %s = %s � ����� �����", __FUNCTION__, strName.c_str(), strTmp.c_str() ); 
		throw PacketErr( "������ �������������� ����" );
	}
}

void CInPacket::GetField( TiXmlElement* pParentElem, const std::string& strName , __int64& iValue )
{
	std::string strTmp;
	GetField( pParentElem, strName, strTmp );
	if( ( 0 == ( iValue = _atoi64( strTmp.c_str() ) ) ) && ( "0" != strTmp ) )
	{
		Log::instance().Trace( 1, "%s: ������ �������������� ���� %s = %s � ����� 64 ������� �����", __FUNCTION__, strName.c_str(), strTmp.c_str() ); 
		throw PacketErr( "������ �������������� ����" );
	}
}

void CInPacket::GetField( TiXmlElement* pParentElem, const std::string& strName , bool& bValue )
{
	std::string strVal;
	GetField( pParentElem, strName, strVal );
	if( 0 == _stricmp( strVal.c_str(), "true" ) )
		bValue = true;
	else if( 0 == _stricmp( strVal.c_str(), "false" ) )
		bValue = false;
	else
	{
		Log::instance().Trace( 1, "%s: ������ �������������� ���� %s = %s ��������� ��������", __FUNCTION__, strName.c_str(), strVal.c_str() ); 
		throw PacketErr( "������ �������������� ����" );
	}
}

void CInPacket::GetField( TiXmlElement* pParentElem, const std::string& strName , time_t& tValue )
{
	std::string strVal;
	GetField( pParentElem, strName, strVal );
	//���� ������� ���������� � ���� ������������������ ����
	if( sizeof( time_t )*2 != strVal.size() )
	{
		Log::instance().Trace( 1, "%s: ������ �������������� ���� %s = %s � �������� ���� time_t", __FUNCTION__, strName.c_str(), strVal.c_str() ); 
		throw PacketErr( "������ �������������� ����" );
	}
	tValue = StrToTimet( strVal );
}

time_t CInPacket::StrToTimet( const std::string& str )
{
	static const char AsciToHex[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
									  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
									  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
									  0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0, 0, 0,
									  0, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF, 0, 0, 0, 0, 0, 0, 0, 0, 0,
									  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
									  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
									  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
									  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
									  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
									  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
									  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
									  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
									  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
									  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
									  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,};
	BYTE bTmp;
	time_t res;
	for( unsigned int i = 0; i < sizeof( time_t ); i++ )
	{
		bTmp = ( AsciToHex[ (BYTE)str[ 2*i ] ] ) * 16;
		bTmp += AsciToHex[ (BYTE)str[ 2*i + 1 ] ];
		memcpy( (BYTE*)&res + i, &bTmp, 1 );
	}
	return res;
}

COutPacket::COutPacket()
{
	m_strPacket += "<?xml version=\"1.0\" encoding=\"windows-1251\"?>\r\n<packet>\r\n</packet>\r\n";
}

void COutPacket::PutField( const std::string& strName, const std::string& strValue )
{
	size_t Pos;
	if( std::string::npos == ( Pos = m_strPacket.find( "</packet>" ) ) )
		throw PacketErr( "�� ������ ����������� ��� </packet>" );
	m_strPacket.insert( Pos, "\t<" + strName + ">" + strValue + "</" + strName + ">\r\n" );
}

void COutPacket::PutField( const std::string& strName, int iValue )
{
	std::stringstream ss;
	ss << iValue;
	PutField( strName, ss.str() );
}

void COutPacket::PutHostRec( const hostRec& Host, const std::string& strProtoName )
{
	std::string strTmp;
	strTmp += "<";strTmp += HOST_TAG;strTmp += " ";strTmp += HOST_ADDR;strTmp += "=\"" + Host.IPNum + "\" ";
	strTmp += " ";strTmp += HOST_NAME;strTmp += "=\"";strTmp += Host.HostName + "\"";
	strTmp += " ";strTmp += PROTO_NAME;strTmp += "=\"";strTmp += strProtoName + "\"";strTmp += ">\r\n";
	for( std::list<fileStr>::const_iterator It = Host.Files.begin(); It != Host.Files.end(); It++ )
	{
		strTmp += "<";strTmp += FILE_TAG;strTmp += ">\r\n";
		strTmp += "<";strTmp += FILE_PATH;strTmp += ">" + It->FileName;strTmp += "</";strTmp += FILE_PATH;strTmp += ">\r\n";
		strTmp += "<";strTmp += FILE_DATE;strTmp += ">";
		strTmp += TimetToStr( It->FDate.UTS );
		strTmp += "</";strTmp += FILE_DATE;strTmp += ">\r\n";
		std::stringstream ss;ss << It->FileSize;
		strTmp += "<";strTmp += FILE_SIZE;strTmp += ">";strTmp += ss.str();strTmp += "</";strTmp += FILE_SIZE;strTmp += ">\r\n";
		strTmp += "</";strTmp += FILE_TAG;strTmp += ">\r\n";
	}
	strTmp += "</";strTmp += HOST_TAG;strTmp += ">\r\n";
	size_t Pos;
	if( std::string::npos == ( Pos = m_strPacket.find( "</packet>" ) ) )
		throw PacketErr( "�� ������ ����������� ��� </packet>" );
	m_strPacket.insert( Pos, strTmp );
}

std::string COutPacket::TimetToStr( time_t time )
{
	static const char HexToAsci[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
	std::string strRes;
	BYTE bTmp;
	for( unsigned int i = 0; i < sizeof(time_t); i++ )
	{
		bTmp = (BYTE)*( (BYTE*)&time + i );
		strRes += HexToAsci[ bTmp/16 ];
		strRes += HexToAsci[ bTmp%16 ];
	}
	return strRes;
}

COutPacket& operator<<( CSocket& sock, COutPacket& packet )
{
	sock.Send( (BYTE*)packet.m_strPacket.c_str(), (int)packet.m_strPacket.size() );
	sock.Send( &CInPacket::GetEndStamp()[0], (int)CInPacket::GetEndStamp().size() );
	return packet;
}
