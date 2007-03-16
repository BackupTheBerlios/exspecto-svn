#ifndef CRITICALSECTION_HPP_
#define CRITICALSECTION_HPP_

#include "windows.h"

class CCriticalSection
{
public:
	CCriticalSection()
	{
		InitializeCriticalSection( &m_cs );
	};	

	~CCriticalSection()
	{
		DeleteCriticalSection( &m_cs );
	};

	void Enter()
	{
		EnterCriticalSection( &m_cs );
	};

	void Leave()
	{
		LeaveCriticalSection( &m_cs );
	};
private:
	CRITICAL_SECTION m_cs;
};

class CLock
{
public:

	CLock( CCriticalSection& cs ):m_cs( cs )
	{
		m_cs.Enter();
	}

	~CLock()
	{
		m_cs.Leave();
	}

private:

	CCriticalSection& m_cs;
};
#endif /*CRITICALSECTION_HPP_*/
