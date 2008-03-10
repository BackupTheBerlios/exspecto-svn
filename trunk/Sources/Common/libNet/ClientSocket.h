//-------------------------------------------------------------------------------------//
//Module: CClientSocket class
//Author: Parshin Dmitry
//Description: 
//-------------------------------------------------------------------------------------//
#ifndef CCLIENTSOCKET_H_
#define CCLIENTSOCKET_H_

#include "Socket.h"

class CClientSocket: public CSocket
{
public:

	CClientSocket( int iType = SOCK_STREAM, bool bBlocking = true );

	~CClientSocket( void );


	void Connect( std::string strAddr, int iPort, int iTimeout = 20000 );

private:
	CClientSocket( const CClientSocket& );
	CClientSocket& operator=( const CClientSocket& );
};

#endif
