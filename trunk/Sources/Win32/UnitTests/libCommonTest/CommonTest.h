#ifndef _COMMON_TEST
#define _COMMON_TEST

#include <cppunit/extensions/HelperMacros.h>
#include "SettingsContainer.h"


class CommonTest : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( CommonTest );
	CPPUNIT_TEST( testSmartPtr );
	CPPUNIT_TEST( testThreadsPool );
	CPPUNIT_TEST( testSettingsContainer );
	CPPUNIT_TEST_EXCEPTION( testSettingsContainerThrow, ParamErr );
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void tearDown();

	void testSmartPtr();

	void testThreadsPool();

	void testSettingsContainer();
	void testSettingsContainerThrow();
};

#endif
