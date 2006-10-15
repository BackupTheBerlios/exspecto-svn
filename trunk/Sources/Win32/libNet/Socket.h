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
			if( 0 == FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM, NULL, iLastError, 0, data, 0, NULL ) )
			{
				data = new char[ sizeof( "Error during formating error message" ) ];
				strcpy( data, "Error during formating error message" );
			} 
		};
		
		virtual ~SocketErr()throw(){};
		
		virtual const char* what() const throw()
		{
		 	return data;	 
		};
		
	private:
	
		char* data;
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
	
	
	//�����������, iType - ��� ������,����� ���� SOCK_STREAM/SOCK_DGRAM
	//			   bBlocking - ��� �������, �� ��������� - �����������
	CSocket( int iType = SOCK_STREAM, bool bBlocking = true )throw( SocketErr );

	//�����������, s - ��������� �������� ::socket �����
	//			   bBlocking - ��� �������, �� ��������� - �����������
	CSocket( SOCKET s, bool bBlocking )throw( SocketErr );

	virtual ~CSocket(void);

	//����� �������� ������
    void Close( void )throw( SocketErr );

	//����� ������� ������,���������� ���-�� ������������ ����
	int Send( void* pBuffer, int iSize )throw( SocketErr );

	//����� ����� ������,���������� ���-�� �������� ����
	int Receive( void* pBuffer, int iBufSize )throw( SocketErr );

	//�����, ��������������� ��� �������(true - �����������,false - ������������� )
	void SetBlocking( bool bIsBlocking );

	//��� ������������� ������������� �������, ����� ���������� true,���� � �������� �����
	//��������� ������ � ����� ����������� �������� Receive
	//Timeout - ����� �������� (���),���� -1,����������� ��������
	bool IsReadyForRead( int iTimeout = -1 )throw( SocketErr );

	//��� ������������� ������������� �������, ����� ���������� true,���� ����� ����� �
	//������
	//Timeout - ����� �������� (���),���� -1,����������� ��������
	bool IsReadyForWrite( int iTimeout = -1 )throw( SocketErr );

protected:

	//���������� ������, ������������ ����������� �� ���� ��������
	SOCKET m_Socket;

	//����,����������� �� ��� ������������ ������� (�����������/�������������)
	bool m_bBlocking;

	//��� ������
	int m_iType;
};

#endif
