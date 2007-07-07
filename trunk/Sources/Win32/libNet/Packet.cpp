//-------------------------------------------------------------------------------------
//Этот файл является частью проекта Exspecto 2006г.
//Module: CPacket class
//Author: Parshin Dmitry
//Description: Класс, реализующий функции для работы с пакетами (парсинг и подготовка)
//-------------------------------------------------------------------------------------
#include "packet.h"
#include "precomp.h"
#include <sstream>
#include "constants.h"
#include <time.h>


CInPacket::CInPacket( BYTE* pbBuf, int iSize ):m_pHostElement( NULL )
											  ,m_pXmlRoot( NULL )
											  ,m_pAddrElement( NULL )
{
	std::string strPacket( pbBuf, pbBuf+iSize );
	if( ( NULL == m_XmlDoc.Parse( strPacket.c_str() ) ) || ( NULL == ( m_pXmlRoot = m_XmlDoc.RootElement() ) ) )
	{
		Log::instance().Trace( 1, "CInPacket::CInPacket: не удалось загрузить xml пакет: %s", strPacket.c_str() );
		throw PacketErr( "Не удалось разобрать входящий xml-пакет" );
	}
}

CInPacket::CInPacket():m_pHostElement( NULL )
					  ,m_pXmlRoot( NULL )
					  ,m_pAddrElement( NULL )
{}

bool CInPacket::Load( BYTE* pbBuf, int iSize )
{
	m_XmlDoc.Clear();
	std::string strPacket( pbBuf, pbBuf+iSize );
	if( ( NULL == m_XmlDoc.Parse( strPacket.c_str() ) ) || ( NULL == ( m_pXmlRoot = m_XmlDoc.RootElement() ) ) )
	{
		Log::instance().Trace( 1, "CInPacket::Load: не удалось загрузить xml пакет: %s", strPacket.c_str() );
		throw PacketErr( "Не удалось разобрать входящий xml-пакет" );
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
	if( NULL == ( m_pAddrElement = m_pXmlRoot->NextSiblingElement( HOST_ADDR ) ) )
	{
		m_pAddrElement = NULL;
		return false;
	}
	strAddress = m_pAddrElement->GetText();
	return true;
}

void CInPacket::GetFirstHostRec( hostRec& Host )
{
	
	m_pHostElement = m_pXmlRoot->FirstChildElement( HOST_TAG );
	if( NULL == m_pHostElement || NULL == m_pHostElement->Attribute( HOST_NAME ) || NULL == m_pHostElement->Attribute( HOST_ADDR ) )
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
	}
}

bool CInPacket::GetNextHostRec( hostRec& Host )
{
	if( NULL == m_pHostElement )
	{
		Log::instance().Trace( 1, "%s: Загрузка информации о файлах не была начата ", __FUNCTION__ ); 
		return false;
	}
	if( NULL == ( m_pHostElement = m_pXmlRoot->NextSiblingElement( HOST_TAG ) ) )
	{
		m_pHostElement = NULL;
		return false;
	}
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
	}
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

void CInPacket::GetField( TiXmlElement* pParentElem, const std::string& strName , __int64& iValue )
{
	std::string strTmp;
	GetField( pParentElem, strName, strTmp );
	if( ( "0" != strTmp ) && ( 0 == ( iValue = _atoi64( strTmp.c_str() ) ) ) )
	{
		Log::instance().Trace( 1, "%s: Ошибка преобразования поля %s = %s в целое 64 битовое число", __FUNCTION__, strName.c_str(), strTmp.c_str() ); 
		throw PacketErr( "Ошибка преобразования поля" );
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
		Log::instance().Trace( 1, "%s: Ошибка преобразования поля %s = %s булевское значение", __FUNCTION__, strName.c_str(), strVal.c_str() ); 
		throw PacketErr( "Ошибка преобразования поля" );
	}
}

void CInPacket::GetField( TiXmlElement* pParentElem, const std::string& strName , time_t& tValue )
{
	std::string strVal;
	GetField( pParentElem, strName, strVal );
	//Поле времени передается в виде последовательности байт
	if( sizeof( time_t ) != strVal.size() )
	{
		Log::instance().Trace( 1, "%s: Ошибка преобразования поля %s = %s в значение типа time_t", __FUNCTION__, strName.c_str(), strVal.c_str() ); 
		throw PacketErr( "Ошибка преобразования поля" );
	}
	std::vector<char> vec( strVal.begin(), strVal.end() );
    memcpy( &tValue, &vec[0], sizeof( time_t ) );
}

COutPacket::COutPacket()
{
	m_strPacket += "<?xml version=\"1.0\" encoding=\"windows-1251\"?>\r\n<packet>\r\n</packet>\r\n";
}

void COutPacket::PutField( const std::string& strName, const std::string& strValue )
{
	size_t Pos;
	if( std::string::npos == ( Pos = m_strPacket.find( "</packet>" ) ) )
		throw PacketErr( "Не найден завершающий тег </packet>" );
	m_strPacket.insert( Pos, "\t<" + strName + ">" + strValue + "</" + strName + ">\r\n" );
}

void COutPacket::PutField( const std::string& strName, int iValue )
{
	std::stringstream ss;
	ss << iValue;
	PutField( strName, ss.str() );
}

void COutPacket::PutHostRec( const hostRec& Host )
{
	m_strPacket += "<";m_strPacket += HOST_TAG;m_strPacket += " ";m_strPacket += HOST_ADDR;m_strPacket += "=" + Host.IPNum;
	m_strPacket += " ";m_strPacket += HOST_NAME;m_strPacket += " ";m_strPacket += Host.HostName + ">";
	for( std::list<fileStr>::const_iterator It = Host.Files.begin(); It != Host.Files.end(); It++ )
	{
		m_strPacket += "<";m_strPacket += FILE_TAG;m_strPacket += ">";
		m_strPacket += "<";m_strPacket += FILE_PATH;m_strPacket += ">" + It->FileName;m_strPacket += "</";m_strPacket += FILE_PATH;m_strPacket += ">";
		m_strPacket += "<";m_strPacket += FILE_DATE;m_strPacket += ">";
		m_strPacket.append( (BYTE*)&It->FDate.UTS, (BYTE*)&It->FDate.UTS + sizeof( time_t ) );
		m_strPacket += "</";m_strPacket += FILE_DATE;m_strPacket += ">";
		std::stringstream ss;ss << It->FileSize;
		m_strPacket += "<";m_strPacket += FILE_SIZE;m_strPacket += ">";m_strPacket += ss.str();m_strPacket += "</";m_strPacket += FILE_SIZE;m_strPacket += ">";
		m_strPacket += "</";m_strPacket += FILE_TAG;m_strPacket += ">\r\n";
	}
	m_strPacket += "</";m_strPacket += HOST_TAG;m_strPacket += ">";

}

COutPacket& operator<<( CSocket& sock, COutPacket& packet )
{
	sock.Send( (BYTE*)packet.m_strPacket.c_str(), packet.m_strPacket.size() );
	sock.Send( &CInPacket::GetEndStamp()[0], CInPacket::GetEndStamp().size() );
	return packet;
}
