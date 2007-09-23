#ifndef _NET_TEST
#define _NET_TEST

#include <cppunit/extensions/HelperMacros.h>


class NetTest : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( NetTest );
	CPPUNIT_TEST( testPacket );
	CPPUNIT_TEST( testPing );
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void tearDown();

	void testPacket();
	void testPing();

};

#endif
