//-------------------------------------------------------------------------------------
//���� ���� �������� ������ ������� Exspecto 2006�.
//Module: CNetBiosScanner class
//Author: Parshin Dmitry
//Description: �����, ����������� ������� ������������ NetBios
//-------------------------------------------------------------------------------------
#pragma once
#include <string>
#include <vector>
#include "CScanner.h"

class CNetBiosScanner: public CScanner
{
public:
	CNetBiosScanner(){};
	virtual ~CNetBiosScanner(){};

	//����������� ����� strAddress, ��������� ������� � vcResList
	void Scan( IN std::string strAddress, OUT std::vector< std::string >& vcResList ); 

	//������� ��� ���������	
	const char* GetProtocolName(){ return "NetBios"; }

protected:

	//����������� �������, ������������ ��� ��������� �����/�����
	void EnumFiles( IN const char* strSharePath, OUT std::vector< std::string >& vcFilesList );
	
};


