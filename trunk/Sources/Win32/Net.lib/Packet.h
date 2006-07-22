#pragma once
#include "Socket.h"

enum enumCommands;

class CPacket
{
public:
	CPacket(void);
	~CPacket(void);

	void BeginCommand( enumCommands Command );

	void AddParam( BYTE* pbParam, int iSize );

	void AddParam( DWORD dwParam );

	void AddParam( std::string strParam );

	bool AddAddress( std::string strAddress );

	void EndCommand();

	void Clear();

	void GetBuffer( OUT BYTE* &pbBuffer, OUT int &iSize );

	void SetBuffer( IN BYTE* pbBuffer, IN int iSize );

	bool GetCommandId( BYTE& pByte );

	bool GetParam( BYTE* pbValue, int iSize );

	bool GetParam( DWORD& dwValue );

	bool GetParam( std::string&	strValue, int iSize );

	bool CPacket::GetAddress( std::string& strAddress );

	friend CPacket& operator <<( CSocket& sock, CPacket& packet );

protected:
	
	void Push( BYTE *pbData, int iSize );

	void Pop( BYTE *pbBuf, int iCount );

	BYTE* m_pbBuf;

	int	m_iDataSize;

	int m_iBufSize;
	
	int m_iOffset;

};


CPacket& operator <<( CSocket& sock, CPacket& packet );
