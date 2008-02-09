//-------------------------------------------------------------------------------------
//���͜�����ޜ��� ������М��ٜ��� ������Ҝ��ۜ�����՜��������� ������М������������ ���ߜ������ޜ��՜��ڜ������ Exspecto 2006����.
//Module: CSocket class
//Author: Parshin Dmitry
//Description: �������ۜ��М������, �������՜��М��ۜ��؜��ל�����������؜��� ���Ҝ��ל��М��؜��ܜ��ޜ��Ԝ��՜��ٜ��������Ҝ��؜��� ���� ������ޜ��ڜ��՜�����М��ܜ��� (���ޜ��ќ�����М��� ���Ԝ��ۜ��� ���ڜ��ۜ��؜��՜��ݜ������ ���� ������՜������Ҝ��՜������� ������М���������)
//-------------------------------------------------------------------------------------

#ifndef CSOCKET_H_
#define CSOCKET_H_

#include "Singleton.hpp"
#include <stdexcept>
#include "CLog.h"
#ifdef WIN32
#include "winsock2.h"
#define socklen_t int
#define ioctl ioctlsocket
#else
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <arpa/inet.h>
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define WSAEMSGSIZE EMSGSIZE
#define WSAEINTR EINTR
#define WSAHOST_NOT_FOUND HOST_NOT_FOUND
typedef int SOCKET;
#endif
#include "Os_Spec.h"


//����������ڜ��ۜ��������՜��ݜ��؜���, ���Ӝ��՜��ݜ��՜������؜���������՜��ܜ������ CSocket ���� ���ڜ��ۜ��М��������М��ܜ���, ���ݜ��М�����ۜ��՜��Ԝ�����������؜��ܜ��� ���ޜ��� ���ݜ��՜��Ӝ���, ���� ���������ݜ��ڜ�����؜�����ܜ��� ���؜��� Tools
class SocketErr: public std::exception
{
public:
	SocketErr( int iLastError )
	{
	    #ifdef WIN32
		if( 0 == FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, iLastError, 0, data, sizeof(data), NULL ) )
		#else
		if( 0 == strerror_r( errno, data, sizeof( data ) ) )
		#endif
		{
			strcpy( data, "Error during formating error message" );
			#ifdef WIN32
			Log::instance().Trace( 0, "SocketErr: ���ޜ�����؜��ќ��ڜ���: %d", GetLastError() );
			#else
			Log::instance().Trace( 0, "SocketErr: ���ޜ�����؜��ќ��ڜ���: %d", errno );
			#endif
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
        SocketDNSErr():SocketErr( "�������Ԝ������՜��� ���ݜ��� ���ݜ��М��ٜ��Ԝ��՜���" ){};
		virtual ~SocketDNSErr()throw(){};
	};

	class SocketRespSizeErr: public SocketErr
	{
		public:
			SocketRespSizeErr():SocketErr( "�������М��ל��ܜ��՜��� ���ޜ�����Ҝ��՜������ ������ۜ��؜�����ڜ��ޜ��� ���Ҝ��՜��ۜ��؜���" ){};
		virtual ~SocketRespSizeErr()throw(){};
	};

	class SocketConnectionLost: public SocketErr
	{
		public:
			SocketConnectionLost():SocketErr( "�������М��ל���������� ������Ҝ�����ל���" ){};
		virtual ~SocketConnectionLost()throw(){};
	};

	//����������������ڜ�������������, ���ޜ��ߜ��؜��������Ҝ��М��������М��� ���М��Ԝ������՜��� ���ڜ��ޜ��ܜ��ߜ�����������՜������� ���� ������՜������
	struct structAddr
	{
		std::string strAddr;
		std::string strName;
		int iPort;
	};


	//�������ޜ��ݜ���������������ڜ�����ޜ���, iType - ������؜��� ������ޜ��ڜ��՜������,���ܜ��ޜ��֜��՜��� ���ќ��������� SOCK_STREAM/SOCK_DGRAM
	//			   bBlocking - ������؜��� ���Ҝ�����ל��ޜ��Ҝ��ޜ���, ���ߜ��� ������ܜ��ޜ��ۜ�����М��ݜ��؜��� - ���ќ��ۜ��ޜ��ڜ��؜���������������؜���
	CSocket( int iType = SOCK_STREAM, bool bBlocking = true );

	//�������ޜ��ݜ���������������ڜ�����ޜ���, s - ������ޜ��ל��Ԝ��М��ݜ��ݜ������ ���������ݜ��ڜ�����؜��՜��� ::socket ������ޜ��ڜ��՜���
	//			   bBlocking - ������؜��� ���Ҝ�����ל��ޜ��Ҝ��ޜ���, ���ߜ��� ������ܜ��ޜ��ۜ�����М��ݜ��؜��� - ���ќ��ۜ��ޜ��ڜ��؜���������������؜���
	CSocket( SOCKET s, bool bBlocking, bool bConnected );

	virtual ~CSocket(void);

	//�������՜�����ޜ��� ���ל��М��ڜ������������؜��� ������ޜ��ڜ��՜������
    void Close( void );

	//�������՜�����ޜ��� ���ߜ��ޜ��������ۜ��ڜ��� ���Ԝ��М��ݜ��ݜ������,���Ҝ��ޜ��ל��Ҝ������М�����М��՜��� ���ڜ��ޜ���-���Ҝ��� ���ޜ�����ߜ������М��Ҝ��ۜ��՜��ݜ��ݜ������ ���ќ��М��ٜ���
	void Send( void* pBuffer, int iSize );

	//�������՜�����ޜ��� ���ߜ������؜�����ܜ��� ���Ԝ��М��ݜ��ݜ������,���Ҝ��ޜ��ל��Ҝ������М�����М��՜��� ���ڜ��ޜ���-���Ҝ��� ���ߜ������؜��ݜ������������ ���ќ��М��ٜ���
	int Receive( void* pBuffer, int iBufSize );

	//�������՜�����ޜ��� ���Ҝ��ޜ��ל��Ҝ���������М��՜��� ���М��Ԝ������՜��� ������Ԝ��М��ۜ��՜��ݜ��ݜ��ޜ��Ӝ��� ������ޜ���������
	structAddr GetRemoteHost();

	//�������՜�����ޜ���, ������������М��ݜ��М��Ҝ��ۜ��؜��Ҝ��М��������؜��� ������؜��� ���Ҝ�����ל��ޜ��Ҝ��ޜ���(true - ���ќ��ۜ��ޜ��ڜ��؜���������������؜���,false - ���ݜ��՜��ќ��ۜ��ޜ��ڜ��؜���������������؜��� )
	void SetBlocking( bool bIsBlocking );

	bool IsConnected()const;

	//������������ ���؜�����ߜ��ޜ��ۜ�����ל��ޜ��Ҝ��М��ݜ��؜��� ���ݜ��՜��ќ��ۜ��ޜ��ڜ��؜���������������؜��� ���Ҝ�����ל��ޜ��Ҝ��ޜ���, ���ܜ��՜�����ޜ��� ���Ҝ��ޜ��ל��Ҝ������М�����М��՜��� true,���՜�����ۜ��� ���� ���ߜ������؜��՜��ܜ��ݜ������ ���ќ��������՜���
	//���ߜ��ޜ�����������ߜ��؜��ۜ��� ���Ԝ��М��ݜ��ݜ������ ���� ���ܜ��ޜ��֜��ݜ��� ���ߜ������ޜ��؜��ל��Ҝ��ޜ��Ԝ��؜������ ���ޜ��ߜ��՜������М�����؜��� Receive
	//Timeout - ���Ҝ������՜��ܜ��� ���ޜ��֜��؜��Ԝ��М��ݜ��؜��� (���ܜ��ڜ���),���՜�����ۜ��� -1,���ќ��՜�����ڜ��ޜ��ݜ��՜�����ݜ��ޜ��� ���ޜ��֜��؜��Ԝ��М��ݜ��؜���
	bool IsReadyForRead( int iTimeout = -1 );

	//������������ ���؜�����ߜ��ޜ��ۜ�����ל��ޜ��Ҝ��М��ݜ��؜��� ���ݜ��՜��ќ��ۜ��ޜ��ڜ��؜���������������؜��� ���Ҝ�����ל��ޜ��Ҝ��ޜ���, ���ܜ��՜�����ޜ��� ���Ҝ��ޜ��ל��Ҝ������М�����М��՜��� true,���՜�����ۜ��� ������ޜ��ڜ��՜��� ���Ӝ��ޜ�����ޜ��� ����
	//���ל��М��ߜ��؜������
	//Timeout - ���Ҝ������՜��ܜ��� ���ޜ��֜��؜��Ԝ��М��ݜ��؜��� (���ܜ��ڜ���),���՜�����ۜ��� -1,���ќ��՜�����ڜ��ޜ��ݜ��՜�����ݜ��ޜ��� ���ޜ��֜��؜��Ԝ��М��ݜ��؜���
	bool IsReadyForWrite( int iTimeout = -1 );

	//�������ޜ��ۜ��������؜������ �������М��ל��ܜ��՜��� ���Ԝ��М��ݜ��ݜ������ ���� ���ќ��������՜�������
	bool GetPendingDataSize( u_long& ulSize );

protected:

	void SetConnected( bool bConnected );

	//�������՜�����ڜ������؜��ߜ�����ޜ��� ������ޜ��ڜ��՜������, ���؜�����ߜ��ޜ��ۜ�����ל�����՜��ܜ������ ���ߜ������М��ڜ�����؜�����՜�����ڜ��� ���Ҝ��� ���Ҝ�����՜��� ���������ݜ��ڜ�����؜������
	SOCKET m_Socket;

	//���Ĝ��ۜ��М���,������ڜ��М��ל�����Ҝ��М��������؜��� ���ݜ��� ������؜��� ���؜�����ߜ��ޜ��ۜ�����ל�����՜��ܜ������ ���Ҝ�����ל��ޜ��Ҝ��ޜ��� (���ќ��ۜ��ޜ��ڜ��؜���������������؜���/���ݜ��՜��ќ��ۜ��ޜ��ڜ��؜���������������؜���)
	bool m_bBlocking;

	//�����؜��� ������ޜ��ڜ��՜������
	int m_iType;

private:

	CSocket( const CSocket& );
	CSocket& operator=( const CSocket& );

	bool m_bConnected;
};


#endif
