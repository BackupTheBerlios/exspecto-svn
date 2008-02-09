#ifndef PING_H_INCLUDED
#define PING_H_INCLUDED

#include "Singleton.hpp"
#include <string>

#ifndef WIN32
#include <arpa/inet.h>
#endif

namespace Tools{

	class CPingHelper
	{
	public:

		CPingHelper();
		~CPingHelper();

		//реализована через вызовы icmp.dll, если использовать RAW сокеты  - понадобятся админские права
		//а пока работает и без них
		bool Ping( const std::string& strHost, unsigned int iTimeout, unsigned int iRequestCount );

	private:
#ifdef WIN32
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
#else
        int in_cksum(u_short *addr, int len);
        void pinger(sockaddr& where, int iDataLen, int iSocket, int iIdent, int iTiming);
        bool pr_pack(char *buf, int cc, struct sockaddr_in *from, int iDataLen, int iIdent);

        int m_iSocket;
#endif

	};
	typedef CSingleton< CPingHelper > PingHelper;
}

#endif // PING_H_INCLUDED
