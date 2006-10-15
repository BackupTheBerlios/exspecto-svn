//-------------------------------------------------------------------------------------
//���� ���� �������� ������ ������� Exspecto 2006�.
//Module: CPacket class
//Author: Parshin Dmitry
//Description: �����, ����������� ������� ��� ������ � �������� (������� � ����������)
//-------------------------------------------------------------------------------------
#ifndef PACKET_H_
#define PACKET_H_

#include "precomp.h"
#include "Socket.h"
#include "../CommonFiles/commands.h"

class CPacket
{
public:
	//������ ����������, ������������ CPacket
	class PacketErr: public std::runtime_error
	{
	public:
		PacketErr( const std::string& strErr )throw():std::runtime_error( strErr ){};
		virtual ~PacketErr()throw(){};
	};

	class PacketFormatErr: public PacketErr
	{
	public:
		PacketFormatErr( const std::string& strErr )throw():PacketErr( strErr ){};
		virtual ~PacketFormatErr()throw(){};
	};

	CPacket(void)throw( PacketErr );
	~CPacket(void);

	//�������� � ����� ������������� �������
	void BeginCommand( enumCommands Command )throw( PacketErr );

	//�������� ��������
	//	pbParam - ����� � �������
	//	iSize - ������ ������
	void AddParam( BYTE* pbParam, int iSize )throw( PacketErr );

	//�������� ��������
	//	dwParam - �������� ���� DWORD
	void AddParam( DWORD dwParam )throw( PacketErr );

	//�������� ��������
	//	strParam - ��������� ��������
	void AddParam( std::string strParam )throw( PacketErr );

	//�������� IP - ����� � �����
	void AddAddress( std::string strAddress )throw( PacketErr, PacketFormatErr );

	//�������� ����� ����� ������
	void EndCommand()throw( PacketErr );

	//�������� �����
	void Clear();

	//�������� ����� ������ ��� �������� ������(����� ������ ��������������� ������)
	void GetBuffer( OUT BYTE* &pbBuffer, OUT int &iSize );

	//���������� ������ ��� �������
	void SetBuffer( IN BYTE* pbBuffer, IN int iSize )throw( PacketErr );

	//�������� ������������� ������� �� �������� �������� � ������
	void GetCommandId( BYTE& pByte )throw( PacketFormatErr );

	//�������� ������ ���� �� ������ �� �������� ��������
	void GetParam( BYTE* pbValue, int iSize )throw( PacketFormatErr );

	//�������� �������� ���� DWORD �� �������� ��������
	void GetParam( DWORD& dwValue )throw( PacketFormatErr );

	//�������� ������ ������ iSize �� ������ �� �������� ��������
	void GetParam( std::string&	strValue, int iSize )throw( PacketFormatErr );

	//�������� IP-����� �� ������ �� �������� ��������
	void GetAddress( std::string& strAddress )throw( PacketFormatErr );

	//��������� ����� � �����
	//���������: CSocket sock;CPacket packet; sock << packet;
	friend CPacket& operator <<( CSocket& sock, CPacket& packet );

protected:
	
	//�������� ������ ���� � ������
	void Push( BYTE *pbData, int iSize )throw( PacketErr );

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
