//-------------------------------------------------------------------------------------
//���� ���� �������� ������ ������� Exspecto 2006�.
//Module: CPacket class
//Author: Parshin Dmitry
//Description: �����, ����������� ������� ��� ������ � �������� (������� � ����������)
//-------------------------------------------------------------------------------------
#pragma once
#include "Socket.h"
#include "../CommonFiles/commands.h"

class CPacket
{
public:
	CPacket(void);
	~CPacket(void);

	//�������� � ����� ������������� �������
	void BeginCommand( enumCommands Command );

	//�������� ��������
	//	pbParam - ����� � �������
	//	iSize - ������ ������
	void AddParam( BYTE* pbParam, int iSize );

	//�������� ��������
	//	dwParam - �������� ���� DWORD
	void AddParam( DWORD dwParam );

	//�������� ��������
	//	strParam - ��������� ��������
	void AddParam( std::string strParam );

	//�������� IP - ����� � �����
	bool AddAddress( std::string strAddress );

	//�������� ����� ����� ������
	void EndCommand();

	//�������� �����
	void Clear();

	//�������� ����� ������ ��� �������� ������(����� ������ ��������������� ������)
	void GetBuffer( OUT BYTE* &pbBuffer, OUT int &iSize );

	//���������� ������ ��� �������
	void SetBuffer( IN BYTE* pbBuffer, IN int iSize );

	//�������� ������������� ������� �� �������� �������� � ������
	bool GetCommandId( BYTE& pByte );

	//�������� ������ ���� �� ������ �� �������� ��������
	bool GetParam( BYTE* pbValue, int iSize );

	//�������� �������� ���� DWORD �� �������� ��������
	bool GetParam( DWORD& dwValue );

	//�������� ������ ������ iSize �� ������ �� �������� ��������
	bool GetParam( std::string&	strValue, int iSize );

	//�������� IP-����� �� ������ �� �������� ��������
	bool CPacket::GetAddress( std::string& strAddress );

	//��������� ����� � �����
	//���������: CSocket sock;CPacket packet; sock << packet;
	friend CPacket& operator <<( CSocket& sock, CPacket& packet );

protected:
	
	//�������� ������ ���� � ������
	void Push( BYTE *pbData, int iSize );

	//�������� ������ ���� �� ������
	void Pop( BYTE *pbBuf, int iCount );

	//���������� �����,���������� �����
	BYTE* m_pbBuf;

	//������ ������,������������ � ������
	int	m_iDataSize;

	//���-�� ������,���������� ��� �����
	int m_iBufSize;
	
	//������� �������� �� ������ ������
	int m_iOffset;
};

//��������� ����� � �����
//���������: CSocket sock;CPacket packet; sock << packet;
CPacket& operator <<( CSocket& sock, CPacket& packet );
