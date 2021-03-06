//-------------------------------------------------------------------------------------//
//���͜�����ޜ��� ������М��ٜ��� ������Ҝ��ۜ�����՜��������� ������М������������ ���ߜ������ޜ��՜��ڜ������ Exspecto 2006����.
//Module: CServerSocket class
//Author: Parshin Dmitry
//Description: �������ۜ��М������, �������՜��М��ۜ��؜��ל�����������؜��� ������՜������Ҝ��՜������ݜ������ ������М��������� ������ޜ��ڜ��՜�����ޜ���
//-------------------------------------------------------------------------------------//
#ifndef SERVERSOCKET_H_
#define SERVERSOCKET_H_

#include "Socket.h"
#include "SmartPtr.hpp"

class CServerSocket: public CSocket
{
public:

	//�������ޜ��ݜ���������������ڜ�����ޜ���
	//iType - ������؜��� ������ޜ��ڜ��՜������, ���Ҝ��ޜ��ל��ܜ��ޜ��֜��ݜ������ ���ל��ݜ��М�����՜��ݜ��؜��� - SOCK_STREAM ���� SOCK_DGRAM
	//bBlocking - ���ќ��ۜ��ޜ��ڜ��؜���������������؜��� ���ۜ��؜��ќ��� ���ݜ��� ���ќ��ۜ��ޜ��ڜ��؜���������������؜��� ������ޜ��ڜ��՜���
	CServerSocket( int iType = SOCK_STREAM, bool bBlocking = true );

	CServerSocket( const CServerSocket& S );

	CServerSocket& operator=( const CServerSocket& );


	~CServerSocket(void);

	//���Ĝ�����ݜ��ڜ�����؜��� "���ߜ������؜��ڜ������՜��ߜ��ۜ��՜��ݜ��؜���" ������ޜ��ڜ��՜������ ���� ���М��Ԝ������՜������, ���ߜ��� ������ܜ��ޜ��ۜ�����М��ݜ��؜��� �������� ������М��ܜ��� ���Ҝ�����ќ��؜������М��՜��� ���М��Ԝ������՜��� ���� ���ߜ��ޜ�������
	void Bind( int iPort = 0, std::string strAddr = "Any" );

	//���Ĝ�����ݜ��ڜ�����؜��� ���ݜ��М�����М��ۜ��� "���ߜ������ޜ�����ۜ��������؜��Ҝ��М��ݜ��؜���", iMaxConn - ���ܜ��М��ڜ�����؜��ܜ��М��ۜ�����ݜ��ޜ��� ���ڜ��ޜ���-���Ҝ��� ������ޜ��՜��Ԝ��؜��ݜ��՜��ݜ��؜���,
	//SOMAXCONN - ���ܜ��М��ڜ�����؜��ܜ��М��ۜ�����ݜ��ޜ��� ���ל��ݜ��М�����՜��ݜ��؜���
	void Listen( int iMaxConn = SOMAXCONN );

	//������������ ���Ҝ�����ל��ޜ��Ҝ��� accept, ������ޜ��ڜ��՜��� ���ќ��ۜ��ޜ��ڜ��؜���������՜��������� ���Ҝ��ߜ��ۜ��ޜ������ ���Ԝ��� ���ߜ��ޜ�����Ҝ��ۜ��՜��ݜ��؜��� ������؜��Ӝ��ݜ��М��ۜ��� ���� ���Ҝ�����ޜ��Ԝ��������՜��� 
	//������ޜ��՜��Ԝ��؜��ݜ��՜��ݜ��؜��� (���ߜ��� ���М��ݜ��М��ۜ��ޜ��Ӝ��؜��� ���� ���������ݜ��ڜ�����؜������ ���؜��� ���ߜ������՜��Ԝ�����Ԝ��������՜��� ���������М���������). ���Ĝ�����ݜ��ڜ�����؜��� ���Ҝ��ޜ��ל��Ҝ������М�����М��՜��� 
	//���ݜ��ޜ��Ҝ������ ������ޜ��ڜ��՜���, ���ڜ��ޜ�����ޜ���������� ���ќ�����Ԝ��՜��� ���؜�����ߜ��ޜ��ۜ�����ל��ޜ��Ҝ��М������������ ���Ԝ��ۜ��� ������Ҝ�����ל��� ���� ���ߜ������؜�����ޜ��՜��Ԝ��؜��ݜ��؜��Ҝ�����՜��ٜ������ ���ܜ��М�����؜��ݜ��ޜ��� 
	//(������؜��������՜��ܜ��� ������ޜ��ל��Ԝ��М������ ���՜��Ӝ��� ������М��ܜ���, ���ߜ������� ���������ߜ��՜�����ݜ��ޜ��� ������ޜ��՜��Ԝ��؜��ݜ��՜��ݜ��؜���). �������ޜ��ۜ��՜��� ���ߜ��ޜ��Ԝ������ޜ��ќ��ݜ������ ���Ԝ��М��ݜ��ݜ������ 
	//���� ���ߜ������؜�����ޜ��՜��Ԝ��؜��ݜ��؜��Ҝ�����՜��ٜ������ ���ܜ��М�����؜��ݜ��� accept ���Ҝ��ޜ��ל��Ҝ������М�����М��՜��� ���� ���ߜ��М������М��ܜ��՜���������� addr 
	//(������؜��� ���М��Ԝ������՜������, IP-���М��Ԝ������՜���, ���ߜ��ޜ�������).
	SmartPtr< CSocket > Accept( structAddr& addr );
private:

};

#endif
