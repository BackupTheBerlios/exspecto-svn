//-------------------------------------------------------------------------------------
//���� ���� �������� ������ ������� Exspecto 2006�.
//Module: CNetBiosScanner class
//Author: Parshin Dmitry
//Description: �����, ����������� ������� ������������ NetBios
//-------------------------------------------------------------------------------------
#ifndef _CNETBIOSSCANNER
#define _CNETBIOSSCANNER

#include "MemLeakDetector.h"
#include <string>
#include <vector>
#include "CScanner.h"
#include "CLog.h"

#define MOD_NAME "NetBios"

class CNetBiosScanner: public CScanner
{
public:
	CNetBiosScanner();
	virtual ~CNetBiosScanner(){};

	//����������� ����� strAddress, ��������� ������� � vcResList
	virtual void Scan( IN std::string strAddress, OUT std::vector< std::string >& vcResList ); 

	//������� ��� ���������	
	virtual const char* GetProtocolName()
	{
		return MOD_NAME;
	}

protected:

	//����������� �������, ������������ ��� ��������� �����/�����
	void EnumFiles( IN const char* strSharePath, OUT std::vector< std::string >& vcFilesList );
	
};

#endif
