#include "ping.h"
#include "CLog.h"
#ifdef WIN32
#include "WinSock2.h"
#endif
#include "Socket.h"

namespace Tools{

#ifdef WIN32

  CPingHelper::CPingHelper()
  {
	WSADATA WSAData;
	if( 0 != ::WSAStartup( MAKEWORD( 1, 1 ), &WSAData ) )
	  throw SocketErr( WSAGetLastError() );

	m_hLib = LoadLibrary("ICMP.DLL");
	if( NULL == m_hLib )
	  throw SocketErr( GetLastError() );

	m_pIcmpCreateFile = (pfnHV)GetProcAddress( m_hLib, "IcmpCreateFile" );
	if( NULL == m_pIcmpCreateFile )
	  {
		FreeLibrary( m_hLib );
		throw SocketErr( GetLastError() );
	  }
	m_pIcmpCloseHandle = (pfnBH)GetProcAddress( m_hLib, "IcmpCloseHandle" );
	if( NULL == m_pIcmpCloseHandle )
	  {
		FreeLibrary( m_hLib );
		throw SocketErr( GetLastError() );
	  }

	m_pIcmpSendEcho = (pfnDHDPWPipPDD)GetProcAddress( m_hLib, "IcmpSendEcho" );
	if( NULL == m_pIcmpSendEcho )
	  {
		FreeLibrary( m_hLib );
		throw SocketErr( GetLastError() );
	  }

  }

  CPingHelper::~CPingHelper()
  {
	FreeLibrary( m_hLib );
	WSACleanup();
  }

  bool CPingHelper::Ping( const std::string& strHost, unsigned int iTimeout, unsigned int iRequestCount )
  {
	char acPingBuffer[32]={0};  
	struct in_addr DestAddress;
	struct hostent* pHostEnt;

	PIP_ECHO_REPLY pIpe = (PIP_ECHO_REPLY)new BYTE[ sizeof(IP_ECHO_REPLY) + sizeof(acPingBuffer)];
	if (pIpe == 0) {
	  throw SocketErr( GetLastError() );
	}

	DestAddress.s_addr = inet_addr(strHost.c_str());
	if (DestAddress.s_addr == INADDR_NONE)
	  pHostEnt = gethostbyname(strHost.c_str());
	else
	  pHostEnt = gethostbyaddr((const char *)&DestAddress, sizeof(struct in_addr), AF_INET);

	if (pHostEnt == NULL) {
	  return false;
	}

	HANDLE hIP;

	hIP = m_pIcmpCreateFile();

	if (hIP == INVALID_HANDLE_VALUE) {
	  delete[] pIpe;
	  throw SocketErr( GetLastError() );
	}



	pIpe->Data = acPingBuffer;
	pIpe->DataSize = sizeof(acPingBuffer);

	bool bRes = true;
	DWORD dwStatus;
	for(unsigned int c=0;c<iRequestCount;c++) {
	  dwStatus = m_pIcmpSendEcho( hIP, *((unsigned int*)pHostEnt->h_addr_list[0]),
								  acPingBuffer, sizeof(acPingBuffer), NULL, pIpe,
								  sizeof(IP_ECHO_REPLY) + sizeof(acPingBuffer), iTimeout );
	  bRes &= (dwStatus == 0)?false:true;
	}
	m_pIcmpCloseHandle(hIP);
	delete[] pIpe;
	return bRes;
  }
#else

#include <sys/param.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <string>


#define	DEFDATALEN	(64 - 8)	/* default data length */
#define	MAXIPLEN	60
#define	MAXICMPLEN	76
#define	MAXPACKET	(65536 - 60 - 8)/* max packet size */
#define	MAXWAIT		10		/* max seconds to wait for response */
#define	NROUTES		9		/* number of record route slots */

#define	A(bit)		rcvd_tbl[(bit)>>3]	/* identify byte in array */
#define	B(bit)		(1 << ((bit) & 0x07))	/* identify bit in byte */
#define	SET(bit)	(A(bit) |= B(bit))
#define	CLR(bit)	(A(bit) &= (~B(bit)))
#define	TST(bit)	(A(bit) & B(bit))


  /*
   * MAX_DUP_CHK is the number of bits in received table, i.e. the maximum
   * number of received sequence numbers we can keep track of.  Change 128
   * to 8192 for complete accuracy...
   */
#define	MAX_DUP_CHK	(8 * 128)
  char rcvd_tbl[MAX_DUP_CHK / 8];
  u_char outpack[MAXPACKET];

  /* protos */

  CPingHelper::CPingHelper()
  {
  }

  CPingHelper::~CPingHelper()
  {
  }

  bool CPingHelper::Ping( const std::string& strHost, unsigned int iTimeout, unsigned int iRequestCount )
  {
	int iSocket;
    struct protoent *proto;
    if (!(proto = getprotobyname("icmp")))
	  {
        Log::instance().Trace( 0, "%s: getprotobyname error icmp", __FUNCTION__ );
        return true;
	  }
    if ((iSocket = socket(AF_INET, SOCK_RAW, proto->p_proto)) < 0)
	  {
        if (errno==EPERM)
		  Log::instance().Trace( 0, "%s: not root", __FUNCTION__ );
        else
		  Log::instance().Trace( 0, "%s: error creating socket for ping", __FUNCTION__ );
	  }

    setuid(getuid());

    /* timing */
    int timing;		/* flag to do timing */
    int ident;		/* process id to identify our packets */
    int datalen = DEFDATALEN;
    struct sockaddr whereto;	/* who to ping */
	struct hostent *hp;
    struct sockaddr_in *to;
    int i;
    int hold, packlen;
    u_char *datap, *packet;
    char hnamebuf[MAXHOSTNAMELEN];

    if( 0 != getuid() )
	  {
		fprintf( stderr,  "ping:not root\r\n" );
		return true;
	  }

    datap = &outpack[8 + sizeof(struct timeval)];

    memset(&whereto, 0, sizeof(struct sockaddr));
    to = (struct sockaddr_in *)&whereto;
    to->sin_family = AF_INET;
    if ( !inet_aton( strHost.c_str(), &to->sin_addr ) )
	  {
        hp = gethostbyname(strHost.c_str());
        if (!hp)
		  {
            (void)fprintf(stderr,
                          "ping: unknown host %s\n", strHost.c_str() );
            return false;
		  }
        to->sin_family = hp->h_addrtype;
        if (hp->h_length > (int)sizeof(to->sin_addr))
		  {
            hp->h_length = sizeof(to->sin_addr);
		  }
        memcpy(&to->sin_addr, hp->h_addr, hp->h_length);
        (void)strncpy(hnamebuf, hp->h_name, sizeof(hnamebuf) - 1);
	  }

    if (datalen >= (int)sizeof(struct timeval)) // can we time transfer
	  timing = 1;
    packlen = datalen + MAXIPLEN + MAXICMPLEN;
    packet = (u_char*)malloc((u_int)packlen);
    if (!packet)
	  {
        (void)fprintf(stderr, "ping: out of memory.\n");
        return false;
	  }
    for (i = 8; i < datalen; ++i)
	  *datap++ = i;

    ident = pthread_self() & 0xFFFF;

	printf( "ident = %d\r\n", ident );

	//* When pinging the broadcast address, you can get a lot of answers.
	//* Doing something so evil is useful if you are trying to stress the
	//* ethernet, or just want to fill the arp cache to get some stuff for
	//* /etc/ethers.
	hold = 48 * 1024;
    (void)setsockopt(iSocket, SOL_SOCKET, SO_RCVBUF, (char *)&hold,
                     sizeof(hold));


    timeval tv;
    gettimeofday( &tv, NULL );
    long long t1 = (long long)(tv.tv_sec) * 1000 + (long long)tv.tv_usec/1000;

    unsigned int j = 0;
    for ( j = 0;j < iRequestCount; j++)
	  {
		pinger(whereto, datalen, iSocket, ident, timing);
        for (;;)
		  {
            struct sockaddr_in from;
            int cc;
            size_t fromlen;
            unsigned long ulSize = 0;
            while( ulSize == 0 )
			  {
                if( ioctl( iSocket, FIONREAD, &ulSize ) == -1 )
				  return false;
                gettimeofday( &tv, NULL );
                long long t2 = (tv.tv_sec) * (long long)1000 + tv.tv_usec/(long long)1000;
                if( (t2-t1) > iTimeout )
				  {
                    printf( "timeout\r\n" );
                    return false;
				  }
                usleep(100000);
			  }
            fromlen = sizeof(from);
            if ( (cc = recvfrom(iSocket, (char *)packet, packlen, 0, (struct sockaddr *)&from, &fromlen) ) < 0)
			  continue;
			int pack_ret;
            if ( -1 == ( pack_ret = pr_pack((char *)packet, cc, &from, datalen, ident) ) )
			  {
                continue;
			  }else if( 1 == pack_ret )
			  return false;
            printf( "packet ok\r\n" );
            break;
			}
		  }
    return true;
  }

  /*
   * pinger --
   * 	Compose and transmit an ICMP ECHO REQUEST packet.  The IP packet
   * will be added on by the kernel.  The ID field is our UNIX process ID,
   * and the sequence number is an ascending integer.  The first 8 bytes
   * of the data portion are used to hold a UNIX "timeval" struct in VAX
   * byte-order, to compute the round-trip time.
   */
  void CPingHelper::pinger( sockaddr& where, int iDataLen, int iSocket, int iIdent, int iTiming )
  {
    long ntransmitted;	/* sequence # for outbound packets = #sent */
	struct icmp *icp;
	int cc;
    int i;

    icp = (struct icmp *)outpack;
    icp->icmp_type = ICMP_ECHO;
    icp->icmp_code = 0;
    icp->icmp_cksum = 0;
    icp->icmp_seq = ntransmitted++;
    icp->icmp_id = iIdent;			/* ID */

    CLR(icp->icmp_seq % MAX_DUP_CHK);

    if (iTiming)
	  (void)gettimeofday((struct timeval *)&outpack[8],
						 (struct timezone *)NULL);

    cc = iDataLen + 8;			/* skips ICMP portion */

    /* compute ICMP checksum here */
    icp->icmp_cksum = in_cksum((u_short *)icp, cc);

    i = sendto(iSocket, (char *)outpack, cc, 0, &where,
               sizeof(struct sockaddr));
    if (i < 0 || i != cc)
	  {
        if (i < 0)
		  perror("ping: sendto");
	  }
  }

  /*
   * pr_pack --
   *	Print out the packet, if it came from us.  This logic is necessary
   * because ALL readers of the ICMP socket get a copy of ALL ICMP packets
   * which arrive ('tis only fair).  This permits multiple copies of this
   * program to be run without having intermingled output (or statistics!).
   */

  //TODO:return codes changed
  //0 = ok
  //1 = host unreachable
  //-1 = not our packet
  int CPingHelper::pr_pack(char *buf, int cc, struct sockaddr_in *from, int iDataLen, int iIdent)
  {
    struct icmp *icp;
    int i;
    u_char *cp,*dp;
    struct iphdr *ip;
    struct timeval tv;
    int hlen, dupflag;

    gettimeofday(&tv, (struct timezone *)NULL);

    /* Check the IP header */
    ip = (struct iphdr *)buf;
    hlen = ip->ihl << 2;
    if (cc < iDataLen + ICMP_MINLEN)
	  {
		printf( "pr_pack: not our packet iDataLen = %d \r\n", iDataLen );
		return -1;
	  }

    /* Now the ICMP part */
    cc -= hlen;
    icp = (struct icmp *)(buf + hlen);
    if (icp->icmp_type == ICMP_ECHOREPLY && icp->icmp_code == 0)
	  {
        if (icp->icmp_id != iIdent)
		  {
			printf( "pr_pack: not our packet, ident = %d, my = %d \r\n", icp->icmp_id, iIdent );
            return -1;			/* 'Twas not our ECHO */
		  }

        if (TST(icp->icmp_seq % MAX_DUP_CHK))
		  {
            dupflag = 1;
		  }else
		  {
            SET(icp->icmp_seq % MAX_DUP_CHK);
            dupflag = 0;
		  }
        cp = (u_char*)icp->icmp_data + 8;
        dp = &outpack[8 + sizeof(struct timeval)];
        for (i = 8; i < iDataLen; ++i, ++cp, ++dp)
		  {
            if (*cp != *dp)
			  return 1;
		  }

	  }else if( icp->icmp_type == ICMP_ECHOREPLY && icp->icmp_code != 0 )
	  {
		printf( "pr_pack:code != 0\r\n" );
		return 1;
	  }
	else
	  {
		printf( "pr_pack:unreachable\r\n" );
        return 1;
	  }
    return 0;
  }

  /*
   * in_cksum --
   *	Checksum routine for Internet Protocol family headers (C Version)
   */
  int CPingHelper::in_cksum(u_short *addr, int len)
  {
    int nleft = len;
    u_short *w = addr;
    int sum = 0;
    u_short answer = 0;
    while (nleft > 1)
	  {
        sum += *w++;
        nleft -= 2;
	  }
    if (nleft == 1)
	  {
        *(u_char *)(&answer) = *(u_char *)w ;
        sum += answer;
	  }
    sum = (sum >> 16) + (sum & 0xffff);	/* add hi 16 to low 16 */
    sum += (sum >> 16);			/* add carry */
    answer = ~sum;				/* truncate to 16 bits */
    return(answer);
  }

#endif
}

