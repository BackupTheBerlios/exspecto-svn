//-------------------------------------------------------------------------------------
//���� ���� �������� ������ ������� Exspecto 2006�.
//Module: CSocket class
//Author: Parshin Dmitry
//Description: �����, ����������� �������������� � �������� (����� ��� ������� � ������� �����)
//-------------------------------------------------------------------------------------

#ifndef CSOCKET_H_
#define CSOCKET_H_

#include "precomp.h"
#include <string>
#include <stdexcept>

class CSocket 
{
public:

	//����������, ������������ CSocket � ��������, ������������ �� ����
	class SocketErr: public std::exception
	{
	public:
		SocketErr( int iLastError )throw()
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

	class SocketDNSErr: public CSocket::SocketErr 
	{
		public:
			SocketDNSErr():CSocket::SocketErr( WSAHOST_NOT_FOUND ){};
		virtual ~SocketDNSErr()throw(){};
	};
	
	class SocketRespSizeErr: public CSocket::SocketErr 
	{
		public:
			SocketRespSizeErr():CSocket::SocketErr( WSAEMSGSIZE ){};
		virtual ~SocketRespSizeErr()throw(){};
	};
	
	class SocketConnectionLost: public CSocket::SocketErr 
	{
		public:
			SocketConnectionLost():CSocket::SocketErr( "������ �����" ){};
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
	CSocket( int iType = SOCK_STREAM, bool bBlocking = true )throw( SocketErr );

	//�����������, s - ��������� �������� ::socket �����
	//			   bBlocking - ��� �������, �� ��������� - �����������
	CSocket( SOCKET s, bool bBlocking, bool bConnected )throw( SocketErr );

	virtual ~CSocket(void);

	//����� �������� ������
    void Close( void )throw( SocketErr );

	//����� ������� ������,���������� ���-�� ������������ ����
	int Send( void* pBuffer, int iSize )throw( SocketErr );

	//����� ����� ������,���������� ���-�� �������� ����
	int Receive( void* pBuffer, int iBufSize )throw( SocketErr );
	
	//����� ��������� ����� ���������� �����
	structAddr GetRemoteHost(); 

	//�����, ��������������� ��� �������(true - �����������,false - ������������� )
	void SetBlocking( bool bIsBlocking );
	
	bool IsConnected()const;

	//��� ������������� ������������� �������, ����� ���������� true,���� � �������� �����
	//��������� ������ � ����� ����������� �������� Receive
	//Timeout - ����� �������� (���),���� -1,����������� ��������
	bool IsReadyForRead( int iTimeout = -1 )throw( SocketErr );

	//��� ������������� ������������� �������, ����� ���������� true,���� ����� ����� �
	//������
	//Timeout - ����� �������� (���),���� -1,����������� ��������
	bool IsReadyForWrite( int iTimeout = -1 )throw( SocketErr );

protected:

	void SetConnected( bool bConnected );

	//���������� ������, ������������ ����������� �� ���� ��������
	SOCKET m_Socket;
	
	//����,����������� �� ��� ������������ ������� (�����������/�������������)
	bool m_bBlocking;

	//��� ������
	int m_iType;

private:
	bool m_bConnected;
};

#endif
