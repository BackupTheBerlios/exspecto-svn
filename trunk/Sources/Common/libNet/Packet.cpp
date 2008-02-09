//-------------------------------------------------------------------------------------
//Этот файл является частью проекта Exspecto 2006г.
//Module: CPacket class
//Author: Parshin Dmitry
//Description: Класс, реализующий функции для работы с пакетами (парсинг и подготовка)
//-------------------------------------------------------------------------------------
#include "Packet.h"
#include "precomp.h"
#include <sstream>
#include "constants.h"
#include <time.h>
#include "Os_Spec.h"

#ifdef WIN32
#define atoll _atoi64
#endif


CInPacket::CInPacket( BYTE* pbBuf, int iSize ):m_pXmlDoc( new TiXmlDocument() )
                                              ,m_pXmlRoot( NULL )
											  ,m_pHostElement( NULL )
											  ,m_pAddrElement( NULL )

{
	std::string strPacket( pbBuf, pbBuf+iSize );
	if( ( NULL == m_pXmlDoc->Parse( strPacket.c_str() ) ) || ( NULL == ( m_pXmlRoot = m_pXmlDoc->RootElement() ) ) )
	{
		Log::instance().Trace( 1, "CInPacket::CInPacket: не удалось загрузить xml пакет: %s", strPacket.c_str() );
		std::string strErr = "Не удалось разобрать входящий xml-пакет: ";
		strErr += m_pXmlDoc->ErrorDesc();
		throw PacketErr( strErr );
	}
}

CInPacket::CInPacket():m_pXmlRoot( NULL )
					  ,m_pHostElement( NULL )
					  ,m_pAddrElement( NULL )
{}

bool CInPacket::Load( BYTE* pbBuf, int iSize )
{
	m_pXmlDoc->Clear();
	std::string strPacket( pbBuf, pbBuf+iSize );
	if( ( NULL == m_pXmlDoc->Parse( strPacket.c_str() ) ) || ( NULL == ( m_pXmlRoot = m_pXmlDoc->RootElement() ) ) )
	{
		Log::instance().Trace( 1, "CInPacket::Load: не удалось загрузить xml пакет: %s", strPacket.c_str() );
		std::string strErr = "Не удалось разобрать входящий xml-пакет: ";
		strErr += m_pXmlDoc->ErrorDesc();
		std::stringstream ss;
		ss << " Row: " << m_pXmlDoc->ErrorRow() << " Col: " << m_pXmlDoc->ErrorCol();
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
		Log::instance().Trace( 1, "%s: Не найдено поле %s либо оно не имеет необходимых атрибутов", __FUNCTION__, HOST_ADDR );
		throw PacketErr( "Не найдено поле" );
	}
	strAddress = m_pAddrElement->GetText();
}

bool CInPacket::GetNextAddress( std::string& strAddress )
{
	if( NULL == m_pAddrElement )
	{
		Log::instance().Trace( 1, "%s: Загрузка информации об адресах не была начата", __FUNCTION__ );
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
		Log::instance().Trace( 1, "%s: Не найдено поле %s либо оно не имеет необходимых атрибутов", __FUNCTION__, HOST_TAG );
		throw PacketErr( "Не найдено поле" );
	}
	GetHostRec( Host );
}

void CInPacket::GetHostRec( hostRec& Host )
{
	if( NULL == m_pHostElement->Attribute( HOST_NAME ) || NULL == m_pHostElement->Attribute( HOST_ADDR ) )
	{
		Log::instance().Trace( 1, "%s: Не найдено поле %s либо оно не имеет необходимых атрибутов", __FUNCTION__, HOST_TAG );
		throw PacketErr( "Не найдено поле" );
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
		Log::instance().Trace( 1, "%s: Загрузка информации о файлах не была начата ", __FUNCTION__ );
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
    memset( &File.FDate, 0, sizeof( fileDate ) );

	GetField( pElem, FILE_PATH, File.FileName );
	GetField( pElem, FILE_SIZE, File.FileSize );
	GetField( pElem, FILE_DATE, File.FDate.UTS );
}

void CInPacket::GetField( TiXmlElement* pParentElem, const std::string& strName, std::string& strValue )
{
	if( NULL == pParentElem )
	{
		Log::instance().Trace( 1, "%s: Пакет для разбора не был загружен", __FUNCTION__ );
		throw PacketErr( "Пакет для разбора не был загружен" );
	}
	TiXmlElement* pTmp = pParentElem->FirstChildElement( strName );
	if( NULL == pTmp )
	{
		Log::instance().Trace( 1, "%s: Не найдено поле %s", __FUNCTION__, strName.c_str() );
		throw PacketErr( "Не найдено поле" );
	}
	strValue = pTmp->GetText();
}

void CInPacket::GetField( TiXmlElement* pParentElem, const std::string& strName , int& iValue )
{
	std::string strTmp;
	GetField( pParentElem, strName, strTmp );
	if( ( 0 == ( iValue = atoi( strTmp.c_str() ) ) ) && ( "0" != strTmp ) )
	{
		Log::instance().Trace( 1, "%s: Ошибка преобразования поля %s = %s в целое число", __FUNCTION__, strName.c_str(), strTmp.c_str() );
		throw PacketErr( "Ошибка преобразования поля" );
	}
}

void CInPacket::GetField( TiXmlElement* pParentElem, const std::string& strName , long long& iValue )
{
	std::string strTmp;
	GetField( pParentElem, strName, strTmp );
	if( ( 0 == ( iValue = atoll( strTmp.c_str() ) ) ) && ( "0" != strTmp ) )
	{
		Log::instance().Trace( 1, "%s: Ошибка преобразования поля %s = %s в целое 64 битовое число", __FUNCTION__, strName.c_str(), strTmp.c_str() );
		throw PacketErr( "Ошибка преобразования поля" );
	}
}

void CInPacket::GetField( TiXmlElement* pParentElem, const std::string& strName , bool& bValue )
{
	std::string strVal;
	GetField( pParentElem, strName, strVal );
	if( 0 == stricmp( strVal.c_str(), "true" ) )
		bValue = true;
	else if( 0 == stricmp( strVal.c_str(), "false" ) )
		bValue = false;
	else
	{
		Log::instance().Trace( 1, "%s: Ошибка преобразования поля %s = %s булевское значение", __FUNCTION__, strName.c_str(), strVal.c_str() );
		throw PacketErr( "Ошибка преобразования поля" );
	}
}

void CInPacket::GetField( TiXmlElement* pParentElem, const std::string& strName , time_t& tValue )
{
	std::string strVal;
	GetField( pParentElem, strName, strVal );
	//Поле времени передается в виде последовательности байт
	if( sizeof( time_t )*2 != strVal.size() )
	{
		Log::instance().Trace( 1, "%s: Ошибка преобразования поля %s = %s в значение типа time_t", __FUNCTION__, strName.c_str(), strVal.c_str() );
		throw PacketErr( "Ошибка преобразования поля" );
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
	Pos = m_strPacket.find( "</packet>" );
	assert( Pos != std::string::npos );
	m_strPacket.insert( Pos, "<" + strName + ">" + strValue + "</" + strName + ">\r\n" );
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
		throw PacketErr( "Не найден завершающий тег </packet>" );
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

