#include "StdAfx.h"
#include ".\scanner.h"
#include "lm.h"

CScanner::CScanner(void)
{
}

CScanner::~CScanner(void)
{
}

void CScanner::Scan( IN std::string strAddress, OUT std::vector< std::string >& vcResList )
{
	BYTE* buf;
	DWORD p1,p2;
	WCHAR serv[1024];
	::MultiByteToWideChar( CP_UTF8, 0, strAddress.c_str(), (int)strAddress.size() + 1, serv, 1024 );
	DWORD handle = 0;
	DWORD res;
	char asd[255];
	do
	{
		res = ::NetShareEnum( (LPSTR)serv, 0, &buf, MAX_PREFERRED_LENGTH, &p1, &p2, &handle );
		SHARE_INFO_0* inf =  (SHARE_INFO_0*)buf;
		for( unsigned int i = 0; i < p1; i++ )
		{
			int buflen = ::WideCharToMultiByte( 1251, 0, (WCHAR*)( inf + i )->shi0_netname, -1, 0, 0, 0, 0);
			int res = ::WideCharToMultiByte( 1251, 0, (WCHAR*)( inf + i )->shi0_netname, -1, asd, buflen, 0, 0 );
			vcResList.push_back( asd );
		}
		::NetApiBufferFree( buf );
	}while( res == ERROR_MORE_DATA );

}