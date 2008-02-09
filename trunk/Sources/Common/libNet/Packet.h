//-------------------------------------------------------------------------------------
//���͜�����ޜ��� ������М��ٜ��� ������Ҝ��ۜ�����՜��������� ������М������������ ���ߜ������ޜ��՜��ڜ������ Exspecto 2006����.
//Module: CPacket class
//Author: Parshin Dmitry
//Description: �������ۜ��М������, �������՜��М��ۜ��؜��ל�����������؜��� ���������ݜ��ڜ�����؜��� ���Ԝ��ۜ��� �������М��ќ��ޜ������ ���� ���ߜ��М��ڜ��՜�����М��ܜ��� (���ߜ��М���������؜��ݜ��� ���� ���ߜ��ޜ��Ԝ��Ӝ��ޜ�����ޜ��Ҝ��ڜ���)
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

//�������ۜ��М��������� ���؜�����ڜ��ۜ��������՜��ݜ��؜���, ���Ӝ��՜��ݜ��՜������؜���������՜��ܜ������ CPacket
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

    //TODO:���ܜ��ޜ��֜��ݜ��� ���� ���ќ��՜��� ������ڜ��М��ל��М�����՜��ۜ��� ���ݜ��� ���� ���ۜ��؜��ݜ�����ڜ������ ���ߜ��ޜ�����՜��ܜ��� ������� ���ߜ��М��Ԝ��М��՜���-���� ���ޜ�����ۜ��М��Ԝ�����؜��ڜ��� ���Ԝ��Ҝ��� �������М��ל���
    //���Ҝ�����ל�����Ҝ��М��՜��������� ���Ԝ��՜���������������ڜ�����ޜ��� TiXmlDocument
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

	//TODO: �������ܜ��� ���ߜ������ޜ�����ޜ��ڜ��ޜ��ۜ��� strProtoName ���Ԝ��ޜ��ۜ��֜��ݜ��� ���ќ��������� ���� ����������������ڜ������������� hostRec
	void PutHostRec( const hostRec& Host, const std::string& strProtoName );

	std::string ToString(){ return m_strPacket; }

private:

	COutPacket( const COutPacket& );
	COutPacket& operator=( const COutPacket& );

	std::string TimetToStr( time_t time );

	std::string m_strPacket;
};

//����������ߜ������М��Ҝ��؜������ ���ߜ��М��ڜ��՜��� ���� ������ޜ��ڜ��՜���
//�������؜��ݜ�����М��ڜ�����؜���: CSocket sock;CPacket packet; sock << packet;
COutPacket& operator <<( CSocket& sock, COutPacket& packet );


#endif
