//-------------------------------------------------------------------------------------
//���� ���� �������� ������ ������� Exspecto 2006�.
//Module: CSocket class
//Author: Parshin Dmitry
//Description: �����, ����������� �������������� � �������� (����� ��� ������� � ������� �����)
//-------------------------------------------------------------------------------------

#ifndef CSOCKET_H_
#define CSOCKET_H_
#include "winsock2.h"
#include "Singleton.hpp"

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

	class CPingHelper
	{
	public:

		CPingHelper();
		~CPingHelper();

		//����������� ����� ������ icmp.dll, ���� ������������ RAW ������  - ����������� ��������� �����
		//� ���� �������� � ��� ���
		bool Ping( const std::string& strHost, unsigned int iTimeout, unsigned int iRequestCount );

	private:

		CPingHelper( const CPingHelper& );
		CPingHelper& operator=( const CPingHelper& );
		
		typedef struct {
			unsigned char Ttl;                                 // Time To Live
			unsigned char Tos;                                 // Type Of Service
			unsigned char Flags;                               // IP header flags
			unsigned char OptionsSize;                         // Size in bytes of options data
			unsigned char *OptionsData;                        // Pointer to options data
		} IP_OPTION_INFORMATION, * PIP_OPTION_INFORMATION;

		typedef struct {
			unsigned int Address;                    // Replying address
			unsigned long  Status;                   // Reply status
			unsigned long  RoundTripTime;            // RTT in milliseconds
			unsigned short DataSize;                 // Echo data size
			unsigned short Reserved;                 // Reserved for system use
			void *Data;                              // Pointer to the echo data
			IP_OPTION_INFORMATION Options;           // Reply options
		} IP_ECHO_REPLY, * PIP_ECHO_REPLY;

		HMODULE m_hLib;

		typedef HANDLE (WINAPI* pfnHV)(VOID);
		typedef BOOL (WINAPI* pfnBH)(HANDLE);
		typedef DWORD (WINAPI* pfnDHDPWPipPDD)(HANDLE, DWORD, LPVOID, WORD,
			PIP_OPTION_INFORMATION, LPVOID, DWORD, DWORD);

		pfnHV m_pIcmpCreateFile;
		pfnBH m_pIcmpCloseHandle;
		pfnDHDPWPipPDD m_pIcmpSendEcho;

	};
	typedef CSingleton< CPingHelper > PingHelper;
}

#endif
