#include <cppunit/config/SourcePrefix.h>
#include "CommonTest.h"
#include "SmartPtr.hpp"
#include "ThreadsPool.h"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( CommonTest );


void CommonTest::setUp()
{
}


void CommonTest::tearDown()
{
}

void CommonTest::testSmartPtr()
{
	SmartPtr< std::string > pStr1;
	CPPUNIT_ASSERT( pStr1.get() == NULL );

	std::string *pst = new std::string();
	SmartPtr< std::string > pStr2( pst );
	CPPUNIT_ASSERT( pStr2.get() != NULL );

	pStr1 = pStr2;
	CPPUNIT_ASSERT( pStr2 == pStr1 );

	SmartPtr< std::string > pStr3( pStr1 );
	CPPUNIT_ASSERT( pStr1 == pStr3 );

	SmartPtr< std::string > pStr4( SmartPtr< std::string >( new std::string( "asd" ) ) );
	CPPUNIT_ASSERT( *pStr4 == "asd" );
	CPPUNIT_ASSERT( pStr4->size() == 3 );
}

void CommonTest::testThreadsPool()
{
	class ThreadTask: public CThreadTask
	{
	public:
		virtual void Execute( const CEvent& CancelEvent )
		{
			
		}
	};

	CThreadsPool pool( 3 );
	CEvent CancelEv;

	pool.CancelAllTasks();
	for( int i = 0; i < 10; i++ )
		pool.AddTask( SmartPtr< CThreadTask >( new ThreadTask ) );
	CPPUNIT_ASSERT( pool.WaitAllComplete( CancelEv ) );

	for( int i = 0; i < 10; i++ )
		pool.AddTask( SmartPtr< CThreadTask >( new ThreadTask ) );
	CPPUNIT_ASSERT( pool.WaitAllComplete( CancelEv ) );

	for( int i = 0; i < 10; i++ )
		pool.AddTask( SmartPtr< CThreadTask >( new ThreadTask ) );
	CancelEv.Set();
	CPPUNIT_ASSERT( !pool.WaitAllComplete( CancelEv ) );
}

//Описание типов параметров
static char* pAgentParamTypes[] = {
	SCHEDULER_ADDRESS, "string",
	LOG_LEVEL,	"int",
	EVENT_PORT, "int",
	SCAN_THREADS_COUNT, "int",
	PING_ON, "bool",
	SCHEDULER_EVENT_PORT, "int",
	LISTEN_PORT, "int",
	RESOLVE_HOST, "bool"
};

void CommonTest::testSettingsContainer()
{
	Settings::instance().SetModule( "Agent", pAgentParamTypes, sizeof( pAgentParamTypes )/sizeof( pAgentParamTypes[0] ) );
	std::string strTmp;
	int iTmp;
	bool bTmp;
	Settings::instance().GetParam( SCHEDULER_ADDRESS, strTmp );
	CPPUNIT_ASSERT( "127.0.0.1" == strTmp );
	Settings::instance().GetParam( LOG_LEVEL, iTmp );
	CPPUNIT_ASSERT( 100 == iTmp );
	Settings::instance().GetParam( RESOLVE_HOST, bTmp );
	CPPUNIT_ASSERT( bTmp );
}

void CommonTest::testSettingsContainerThrow()
{
	Settings::instance().SetModule( "Agent", pAgentParamTypes, sizeof( pAgentParamTypes )/sizeof( pAgentParamTypes[0] ) );
	Settings::instance().ClearParams();
	int iTmp;
	Settings::instance().GetParam( LOG_LEVEL, iTmp );
}
