//-------------------------------------------------------------------------------------
//���� ���� �������� ������ ������� Exspecto 2006�.
//Module: CScanner class
//Author: Parshin Dmitry
//Description: �����, ����������� ������� ������������
//-------------------------------------------------------------------------------------
#pragma once
#include <string>
#include <vector>

class CScanner
{
public:
	CScanner(void);
	~CScanner(void);

	//����������� ����� strAddress, ��������� ������� � vcResList
	void Scan( IN std::string strAddress, OUT std::vector< std::string >& vcResList ); 

protected:

	//����������� �������, ������������ ��� ��������� �����/�����
	void EnumFiles( IN const char* strSharePath, OUT std::vector< std::string >& vcFilesList );

	
};
