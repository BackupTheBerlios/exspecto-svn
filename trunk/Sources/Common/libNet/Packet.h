//-------------------------------------------------------------------------------------
//œô¨Íœô¨âœô¨Þœô¨â œô¨äœô¨Ðœô¨Ùœô¨Û œô¨ïœô¨Òœô¨Ûœô¨ïœô¨Õœô¨âœô¨áœô¨ï œô¨çœô¨Ðœô¨áœô¨âœô¨ìœô¨î œô¨ßœô¨àœô¨Þœô¨Õœô¨Úœô¨âœô¨Ð Exspecto 2006œô¨Ó.
//Module: CPacket class
//Author: Parshin Dmitry
//Description: œô¨ºœô¨Ûœô¨Ðœô¨áœô¨á, œô¨àœô¨Õœô¨Ðœô¨Ûœô¨Øœô¨×œô¨ãœô¨îœô¨éœô¨Øœô¨Ù œô¨äœô¨ãœô¨Ýœô¨Úœô¨æœô¨Øœô¨Ø œô¨Ôœô¨Ûœô¨ï œô¨àœô¨Ðœô¨Ñœô¨Þœô¨âœô¨ë œô¨á œô¨ßœô¨Ðœô¨Úœô¨Õœô¨âœô¨Ðœô¨Üœô¨Ø (œô¨ßœô¨Ðœô¨àœô¨áœô¨Øœô¨Ýœô¨Ó œô¨Ø œô¨ßœô¨Þœô¨Ôœô¨Óœô¨Þœô¨âœô¨Þœô¨Òœô¨Úœô¨Ð)
//-------------------------------------------------------------------------------------
#ifndef PACKET_H_
#define PACKET_H_

#include "Socket.h"
#include "precomp.h"
#define TIXML_USE_STL
#include "tinyxml.h"
#include "SmartPtr.hpp"
#include <string>
#include "CDBProvider.h"

//œô¨ºœô¨Ûœô¨Ðœô¨áœô¨áœô¨ë œô¨Øœô¨áœô¨Úœô¨Ûœô¨îœô¨çœô¨Õœô¨Ýœô¨Øœô¨Ù, œô¨Óœô¨Õœô¨Ýœô¨Õœô¨àœô¨Øœô¨àœô¨ãœô¨Õœô¨Üœô¨ëœô¨× CPacket
class PacketErr: public std::runtime_error
{
public:
	PacketErr( const std::string& strErr ):std::runtime_error( strErr ){};
	virtual ~PacketErr()throw(){};
};

class CInPacket
{
public:

	CInPacket( BYTE* pbBuf, int iSize );

	CInPacket();

	virtual ~CInPacket()
	{};

	bool Load( BYTE* pbBuf, int iSize );

	void GetField( const std::string& strName, std::string& strValue );
	void GetField( const std::string& strName, int& iValue );
	void GetField( const std::string& strName, bool& bValue );

	void GetFirstAddress( std::string& strAddress );
	bool GetNextAddress( std::string& strAddress );

	void GetFirstHostRec( hostRec& Host );
	bool GetNextHostRec( hostRec& Host );

	static std::vector<BYTE> GetEndStamp()
	{
		const static BYTE EndStamp[] = { 0, 0x10, 0x13, 0 };
		return std::vector<BYTE>( EndStamp, EndStamp + sizeof( EndStamp ) );
	}

private:

	CInPacket( const CInPacket& );
	CInPacket& operator=( const CInPacket& );

	void GetHostRec( hostRec& Host );

	void GetFileStruct( TiXmlElement* pElem, fileStr& File );

	void GetField( TiXmlElement* pParentElem, const std::string& strName , std::string& strValue );

	void GetField( TiXmlElement* pParentElem, const std::string& strName , long long& iValue );

	void GetField( TiXmlElement* pParentElem, const std::string& strName , int& iValue );

	void GetField( TiXmlElement* pParentElem, const std::string& strName , bool& bValue );

	void GetField( TiXmlElement* pParentElem, const std::string& strName , time_t& bValue );

	time_t StrToTimet( const std::string& str );

    //TODO:œô¨Üœô¨Þœô¨Öœô¨Ýœô¨Þ œô¨Ø œô¨Ñœô¨Õœô¨× œô¨ãœô¨Úœô¨Ðœô¨×œô¨Ðœô¨âœô¨Õœô¨Ûœô¨ï œô¨Ýœô¨Þ œô¨Ò œô¨Ûœô¨Øœô¨Ýœô¨ãœô¨Úœô¨áœô¨Õ œô¨ßœô¨Þœô¨çœô¨Õœô¨Üœô¨ã œô¨âœô¨Þ œô¨ßœô¨Ðœô¨Ôœô¨Ðœô¨Õœô¨â-œô¨Ò œô¨Þœô¨âœô¨Ûœô¨Ðœô¨Ôœô¨çœô¨Øœô¨Úœô¨Õ œô¨Ôœô¨Òœô¨Ð œô¨àœô¨Ðœô¨×œô¨Ð
    //œô¨Òœô¨ëœô¨×œô¨ëœô¨Òœô¨Ðœô¨Õœô¨âœô¨áœô¨ï œô¨Ôœô¨Õœô¨áœô¨âœô¨àœô¨ãœô¨Úœô¨âœô¨Þœô¨à TiXmlDocument
	std::auto_ptr<TiXmlDocument> m_pXmlDoc;

	TiXmlElement* m_pXmlRoot;

	TiXmlElement* m_pHostElement;

	TiXmlElement* m_pAddrElement;

};

class COutPacket
{
	friend COutPacket& operator<<( CSocket& sock, COutPacket& packet );
public:

	COutPacket();

	void PutField( const std::string& strName, const std::string& strValue );

	void PutField( const std::string& strName, int iValue );

	//TODO: œô¨¸œô¨Üœô¨ï œô¨ßœô¨àœô¨Þœô¨âœô¨Þœô¨Úœô¨Þœô¨Ûœô¨Ð strProtoName œô¨Ôœô¨Þœô¨Ûœô¨Öœô¨Ýœô¨Þ œô¨Ñœô¨ëœô¨âœô¨ì œô¨Ò œô¨áœô¨âœô¨àœô¨ãœô¨Úœô¨âœô¨ãœô¨àœô¨Õ hostRec
	void PutHostRec( const hostRec& Host, const std::string& strProtoName );

	std::string ToString(){ return m_strPacket; }

private:

	COutPacket( const COutPacket& );
	COutPacket& operator=( const COutPacket& );

	std::string TimetToStr( time_t time );

	std::string m_strPacket;
};

//œô¨¾œô¨âœô¨ßœô¨àœô¨Ðœô¨Òœô¨Øœô¨âœô¨ì œô¨ßœô¨Ðœô¨Úœô¨Õœô¨â œô¨Ò œô¨áœô¨Þœô¨Úœô¨Õœô¨â
//œô¨Áœô¨Øœô¨Ýœô¨âœô¨Ðœô¨Úœô¨áœô¨Øœô¨á: CSocket sock;CPacket packet; sock << packet;
COutPacket& operator <<( CSocket& sock, COutPacket& packet );


#endif
