#pragma once
#include "commands.h"
#include "Socket.h"

class CPacket
{
public:
	CPacket(void);
	~CPacket(void);

	void BeginCommand( enumCommands Command );

	void AddBinParam( BYTE* pbParam, int iSize );

	void AddDwordParam( DWORD dwParam );

	void AddStrParam( std::string strParam );

	void EndCommand();

	void Clear();

	void GetBuffer( OUT BYTE* pbBuffer, OUT int &iSize );

	void SetBuffer( IN BYTE* pbBuffer, IN int iSize );

	friend CPacket& operator <<( CSocket& sock, CPacket& packet );

protected:
	
	void Push( BYTE* pbData, int iSize );

	BYTE* m_pbBuf;

	int	m_iDataSize;

	int m_iBufSize;

};


CPacket& operator <<( CSocket& sock, CPacket& packet );
