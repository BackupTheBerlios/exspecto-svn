#ifndef EVENT_HPP_
#define EVENT_HPP_

class CEvent
{
public:
	CEvent( bool bAutoReset = true, BOOL bInitialState = 0 ):m_bAutoReset( bAutoReset )
	{
		if( m_bAutoReset )
			m_hEv = CreateEvent( 0,0,0,0 );
		else
			m_hEv = CreateEvent( 0,1,bInitialState,0 );
	};
	
	~CEvent()
	{
		CloseHandle( m_hEv );
	}
	
	void Set()
	{
		SetEvent( m_hEv );
	}
	
	void Reset()
	{
		ResetEvent( m_hEv );	
	}
	
	operator HANDLE()const
	{
		return m_hEv;
	}

	
private:

	//CEvent( CEvent& ev );
	CEvent( const CEvent& ev );
	//CEvent& operator=( CEvent& ev );
	CEvent& operator=( const CEvent& ev );

	bool m_bAutoReset;

	HANDLE m_hEv;
};

#endif /*EVENT_HPP_*/
