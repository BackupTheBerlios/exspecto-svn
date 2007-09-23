#include "NetTest.h"
#include <cppunit/config/SourcePrefix.h>

#include "Packet.h"
#include <vector>
#include <sstream>
#include "ping.h"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( NetTest );


void NetTest::setUp()
{
}


void NetTest::tearDown()
{
}

void NetTest::testPacket()
{
	{
		//Проверяем методы формирования пакета
		std::string strStamp = "<?xml version=\"1.0\" encoding=\"windows-1251\"?>\r\n<packet>\r\n"
								"<f1>12</f1>\r\n"
								"<f2>str</f2>\r\n"
								"<host ipaddress=\"127.0.0.1\"  hostname=\"local\" protoname=\"netbios\">\r\n"
								"<file>\r\n"
								"<filepath>tmp.tmp</filepath>\r\n"
								"<filedate>BC040000</filedate>\r\n"
								"<filesize>10</filesize>\r\n"
								"</file>\r\n"
								"</host>\r\n"
								"</packet>\r\n";
		COutPacket p;
		p.PutField( "f1", 12 );
		p.PutField( "f2", "str" );
		hostRec rec;
		rec.IPNum = "127.0.0.1";
		rec.HostName = "local";
		fileStr file;
		file.FileSize = 10;
		file.FileName = "tmp.tmp";
		file.FDate.UTS = 1212;
		rec.Files.push_back( file );
		p.PutHostRec( rec, "netbios" );
		std::string strP = p.ToString();
		CPPUNIT_ASSERT( strP == strStamp );
	}
	{		std::vector< std::string > vecAddresses;		for( int i = 1; i < 21; i++ )		{			std::stringstream ss;			ss << "172.16.1." << i;			vecAddresses.push_back( ss.str() );		}		std::string strPacket = "<?xml version=\"1.0\" encoding=\"windows-1251\"?>"
								"<packet>"								"<commandid>StartScan</commandid>"
								"<count>1524</count>"
								"<bool>true</bool>"
								"<ipaddress>172.16.1.1</ipaddress><ipaddress>172.16.1.2</ipaddress><ipaddress>172.16.1.3</ipaddress><ipaddress>172.16.1.4</ipaddress><ipaddress>172.16.1.5</ipaddress><ipaddress>172.16.1.6</ipaddress><ipaddress>172.16.1.7</ipaddress><ipaddress>172.16.1.8</ipaddress>"
                                "<ipaddress>172.16.1.9</ipaddress><ipaddress>172.16.1.10</ipaddress><ipaddress>172.16.1.11</ipaddress><ipaddress>172.16.1.12</ipaddress><ipaddress>172.16.1.13</ipaddress><ipaddress>172.16.1.14</ipaddress><ipaddress>172.16.1.15</ipaddress><ipaddress>172.16.1.16</ipaddress>"
                                "<ipaddress>172.16.1.17</ipaddress><ipaddress>172.16.1.18</ipaddress><ipaddress>172.16.1.19</ipaddress><ipaddress>172.16.1.20</ipaddress>"
								"<host ipaddress=\"127.0.0.1\"  hostname=\"local\" protoname=\"netbios\"><file><filepath>tmp.tmp</filepath><filedate>BC040000</filedate><filesize>10</filesize></file></host>"
								"<host ipaddress=\"127.0.0.2\"  hostname=\"local2\" protoname=\"netbios2\"><file><filepath>tmp2.tmp2</filepath><filedate>BC040000</filedate><filesize>102</filesize></file></host></packet>\r\n";
		//Проверяем методы разбора входящих пакетов
		CInPacket p( (BYTE*)strPacket.c_str(), (int)strPacket.length() );

		std::string strTmp;
		p.GetFirstAddress( strTmp );
		int j = 0;
		do{
			CPPUNIT_ASSERT( strTmp == vecAddresses[j++] );
		}while( p.GetNextAddress( strTmp ) );

		hostRec hostTmp;
		p.GetFirstHostRec( hostTmp );
		CPPUNIT_ASSERT( hostTmp.IPNum == "127.0.0.1" );
		CPPUNIT_ASSERT( hostTmp.HostName == "local" );
		fileStr file = hostTmp.Files.back();
		CPPUNIT_ASSERT( file.FileSize == 10 );
		CPPUNIT_ASSERT( file.FileName == "tmp.tmp" );
		CPPUNIT_ASSERT( file.FDate.UTS == 1212 );
		p.GetNextHostRec( hostTmp );
		CPPUNIT_ASSERT( hostTmp.IPNum == "127.0.0.2" );
		CPPUNIT_ASSERT( hostTmp.HostName == "local2" );
		file = hostTmp.Files.back();
		CPPUNIT_ASSERT( file.FileSize == 102 );
		CPPUNIT_ASSERT( file.FileName == "tmp2.tmp2" );
		CPPUNIT_ASSERT( file.FDate.UTS == 1212 );

		CPPUNIT_ASSERT( false == p.GetNextHostRec( hostTmp ) );

		std::string strPacket2 = "<?xml version=\"1.0\" encoding=\"windows-1251\"?>"
								"<packet>"								"<host ipaddress=\"127.0.0.2\"  hostname=\"local2\" protoname=\"netbios2\">"
								"</host>"
								"</packet>\r\n";

		CInPacket EmpP( (BYTE*)strPacket2.c_str(), (int)strPacket.size() );
		EmpP.GetFirstHostRec( hostTmp );
		CPPUNIT_ASSERT( "127.0.0.2" == hostTmp.IPNum );
		CPPUNIT_ASSERT( "local2" == hostTmp.HostName );
		CPPUNIT_ASSERT( 0 == hostTmp.Files.size() );


		int iTmp;
		bool bTmp;
		p.GetField( "commandid", strTmp );
		CPPUNIT_ASSERT( "StartScan" == strTmp );
		p.GetField( "count", iTmp );
		CPPUNIT_ASSERT( 1524 == iTmp );
		p.GetField( "bool", bTmp );
		CPPUNIT_ASSERT( true == bTmp );
	}
}

void NetTest::testPing()
{
	Tools::CPingHelper ping;
	CPPUNIT_ASSERT( true == ping.Ping( "127.0.0.1", 1000, 1 ) );
}
