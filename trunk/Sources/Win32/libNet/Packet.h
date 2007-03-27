//-------------------------------------------------------------------------------------
//���� ���� �������� ������ ������� Exspecto 2006�.
//Module: CPacket class
//Author: Parshin Dmitry
//Description: �����, ����������� ������� ��� ������ � �������� (������� � ����������)
//-------------------------------------------------------------------------------------
#ifndef PACKET_H_
#define PACKET_H_

#include "Socket.h"
#include "precomp.h"
#include "../CommonFiles/commands.h"

class CPacket
{
public:
	//������ ����������, ������������ CPacket
	class PacketErr: public std::runtime_error
	{
	public:
		PacketErr( const std::string& strErr ):std::runtime_error( strErr ){};
		virtual ~PacketErr()throw(){};
	};

	class PacketFormatErr: public PacketErr
	{
	public:
		PacketFormatErr( const std::string& strErr ):PacketErr( strErr ){};
		virtual ~PacketFormatErr()throw(){};
	};

	CPacket(void);
	~CPacket(void);

	//�������� � ����� ������������� �������
	void BeginCommand( enumCommands Command );

	//�������� ��������
	//	pbParam - ����� � �������
	//	iSize - ������ ������
	void AddParam( const BYTE* pbParam, int iSize );

	//�������� ��������
	//	dwParam - �������� ���� DWORD
	void AddParam( DWORD dwParam );

	//�������� ��������
	//	strParam - ��������� ��������
	void AddParam( std::string strParam );

	//�������� IP - ����� � �����
	void AddAddress( std::string strAddress );

	//�������� ����� ����� ������
	void EndCommand();

	//�������� �����
	void Clear();

	//�������� ����� ������ ��� �������� ������(����� ������ ��������������� ������)
	void GetBuffer( OUT BYTE* &pbBuffer, OUT int &iSize );

	//���������� ������ ��� �������
	void SetBuffer( IN BYTE* pbBuffer, IN int iSize );

	//�������� ������������� ������� �� �������� �������� � ������
	void GetCommandId( BYTE& pByte );

	//�������� ������ ���� �� ������ �� �������� ��������
	void GetParam( BYTE* pbValue, int iSize );

	//�������� �������� ���� DWORD �� �������� ��������
	void GetParam( DWORD& dwValue );

	//�������� ������ ������ iSize �� ������ �� �������� ��������
	void GetParam( std::string&	strValue, int iSize );

	//�������� IP-����� �� ������ �� �������� ��������
	void GetAddress( std::string& strAddress );
	
	//�������� ��������� ��������� ������ ��� ���
	bool IsDone();

	//��������� ����� � �����
	//���������: CSocket sock;CPacket packet; sock << packet;
	friend CPacket& operator <<( CSocket& sock, CPacket& packet );

protected:
	
	//�������� ������ ���� � ������
	void Push( const BYTE *pbData, int iSize );

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

#endif
