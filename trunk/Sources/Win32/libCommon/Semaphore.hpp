#ifndef SEMAPHORE_HPP_
#define SEMAPHORE_HPP_

class CSemaphore
{
public:
	CSemaphore( long iResourceCount = LONG_MAX, long iInitCount = 0 )
	{
		m_hSem = CreateSemaphore( NULL, iInitCount, iResourceCount, NULL );
	};
	
	~CSemaphore()
	{
		CloseHandle( m_hSem );
	}
	
	void Release()
	{
		ReleaseSemaphore( m_hSem, 1, NULL );
	}
	
	
	operator HANDLE()const
	{
		return m_hSem;
	}

	
private:

	CSemaphore( const CSemaphore& );

	CSemaphore& operator=( const CSemaphore& );

	bool m_bAutoReset;

	HANDLE m_hSem;
};

#endif /*EVENT_HPP_*/
