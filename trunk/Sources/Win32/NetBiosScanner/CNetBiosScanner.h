//-------------------------------------------------------------------------------------
//���� ���� �������� ������ ������� Exspecto 2006�.
//Module: CNetBiosScanner class
//Author: Parshin Dmitry
//Description: �����, ����������� ������� ������������ NetBios
//-------------------------------------------------------------------------------------
#ifndef _CNETBIOSSCANNER
#define _CNETBIOSSCANNER

#include <string>
#include <vector>
#include "CScanner.h"
#include "CLog.h"

class CNetBiosScanner: public CScanner
{
public:
	CNetBiosScanner();
	virtual ~CNetBiosScanner(){};

	//����������� ����� strAddress, ��������� ������� � vcResList
	void Scan( IN std::string strAddress, OUT std::vector< std::string >& vcResList ); 

	//������� ��� ���������	
	const char* GetProtocolName()
	{
		log.Trace( 1, "dsfs %s", m_strProtoName ); 
		return m_strProtoName; 
	}

protected:

	//����������� �������, ������������ ��� ��������� �����/�����
	void EnumFiles( IN const char* strSharePath, OUT std::vector< std::string >& vcFilesList );
	
	char m_strProtoName[ 8 ];
	
};

#endif
