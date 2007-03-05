#ifndef EVENT_HPP_
#define EVENT_HPP_

class CEvent
{
public:
	CEvent( bool bAutoReset = true )
	{
		if( bAutoReset )
			m_hEv = CreateEvent( 0,0,0,0 );
		else
			m_hEv = CreateEvent( 0,1,0,0 );
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

	HANDLE m_hEv;
};

#endif /*EVENT_HPP_*/
