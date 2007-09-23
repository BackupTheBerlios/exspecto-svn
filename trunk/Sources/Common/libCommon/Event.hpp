#ifndef EVENT_HPP_
#define EVENT_HPP_

class CEvent
{
public:
	CEvent( bool bAutoReset = true, bool bInitialState = false ):m_sem(1)
																,m_bAutoReset( bAutoReset )
																,m_bInitialState( bInitialState )
	{
		if( m_bAutoReset && bInitialState || m_bAutoReset )
			m_sem.wait();
	}

	~CEvent()
	{}

	void Set()
	{
		m_sem.post();
	}

	void Reset()
	{
		m_sem.trywait();
	}

	void Wait()
	{
		m_sem.wait();
		if( !m_bAutoReset )
			m_sem.post();
	}

	bool Wait( int iMilliseconds )
	{
	    //TODO:
	    if( m_sem.trywait() )
	    {
            if( !m_bAutoReset )
                m_sem.post();
            return true;
	    }
        pt::psleep( iMilliseconds );
        if( m_sem.trywait() )
        {
            if( !m_bAutoReset )
                m_sem.post();
            return true;
        }
        return false;
	}
private:
	CEvent( const CEvent& );
	CEvent& operator=( const CEvent& );

	pt::semaphore m_sem;

	bool m_bAutoReset;
	bool m_bInitialState;
};

#endif /*EVENT_HPP_*/
