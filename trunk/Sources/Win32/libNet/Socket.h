//-------------------------------------------------------------------------------------
//���� ���� �������� ������ ������� Exspecto 2006�.
//Module: CSocket class
//Author: Parshin Dmitry
//Description: �����, ����������� �������������� � �������� (����� ��� ������� � ������� �����)
//-------------------------------------------------------------------------------------

#ifndef CSOCKET_H_
#define CSOCKET_H_
#include "winsock2.h"
#include "icmpapi.h"

#include "precomp.h"

//����������, ������������ CSocket � ��������, ������������ �� ����, � ��������� �� Tools
class SocketErr: public std::exception
{
public:
	SocketErr( int iLastError )
	{
		if( 0 == FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, iLastError, 0, data, sizeof(data), NULL ) )
		{
			strcpy( data, "Error during formating error message" );
			Log::instance().Trace( 0, "SocketErr: ������: %d", GetLastError() );
		}
	};

	SocketErr( const std::string& Msg )
	{
		strcpy( data, Msg.c_str() );
	};

	virtual ~SocketErr()throw(){};

	virtual const char* what() const throw()
	{
		return data;	 
	};

private:

	char data[1024];
};


class CSocket 
{
public:


	class SocketDNSErr: public SocketErr 
	{
		public:
			SocketDNSErr():SocketErr( WSAHOST_NOT_FOUND ){};
		virtual ~SocketDNSErr()throw(){};
	};
	
	class SocketRespSizeErr: public SocketErr 
	{
		public:
			SocketRespSizeErr():SocketErr( WSAEMSGSIZE ){};
		virtual ~SocketRespSizeErr()throw(){};
	};
	
	class SocketConnectionLost: public SocketErr 
	{
		public:
			SocketConnectionLost():SocketErr( "������ �����" ){};
		virtual ~SocketConnectionLost()throw(){};
	};

	//���������, ����������� ����� ���������� � ����
	struct structAddr
	{
		std::string strAddr;
		std::string strName;
		int iPort;
	};	
	
	
	//�����������, iType - ��� ������,����� ���� SOCK_STREAM/SOCK_DGRAM
	//			   bBlocking - ��� �������, �� ��������� - �����������
	CSocket( int iType = SOCK_STREAM, bool bBlocking = true );

	//�����������, s - ��������� �������� ::socket �����
	//			   bBlocking - ��� �������, �� ��������� - �����������
	CSocket( SOCKET s, bool bBlocking, bool bConnected );

	virtual ~CSocket(void);

	//����� �������� ������
    void Close( void );

	//����� ������� ������,���������� ���-�� ������������ ����
	int Send( void* pBuffer, int iSize );

	//����� ����� ������,���������� ���-�� �������� ����
	int Receive( void* pBuffer, int iBufSize );
	
	//����� ��������� ����� ���������� �����
	structAddr GetRemoteHost(); 

	//�����, ��������������� ��� �������(true - �����������,false - ������������� )
	void SetBlocking( bool bIsBlocking );
	
	bool IsConnected()const;

	//��� ������������� ������������� �������, ����� ���������� true,���� � �������� �����
	//��������� ������ � ����� ����������� �������� Receive
	//Timeout - ����� �������� (���),���� -1,����������� ��������
	bool IsReadyForRead( int iTimeout = -1 );

	//��� ������������� ������������� �������, ����� ���������� true,���� ����� ����� �
	//������
	//Timeout - ����� �������� (���),���� -1,����������� ��������
	bool IsReadyForWrite( int iTimeout = -1 );

protected:

	void SetConnected( bool bConnected );

	//���������� ������, ������������ ����������� �� ���� ��������
	SOCKET m_Socket;
	
	//����,����������� �� ��� ������������ ������� (�����������/�������������)
	bool m_bBlocking;

	//��� ������
	int m_iType;

private:

	CSocket( const CSocket& );
	CSocket& operator=( const CSocket& );

	bool m_bConnected;
};
//-----------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------Tools-------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------

namespace Tools{

	//����������� ����� ������ icmp.dll, ���� ������������ RAW ������  - ����������� ��������� �����
	//� ���� �������� � ��� ���
	bool Ping( const std::string& strHost, unsigned int iTimeout, unsigned int iRequestCount );
}

#endif
