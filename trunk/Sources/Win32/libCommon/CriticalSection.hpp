#ifndef CRITICALSECTION_HPP_
#define CRITICALSECTION_HPP_

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

#endif /*CRITICALSECTION_HPP_*/
