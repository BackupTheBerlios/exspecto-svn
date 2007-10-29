#include "Event.h"



CEvent::CEvent( bool bAutoreset, bool bInitialState ):m_bAutoReset( bAutoreset )
                                                     ,m_bInitialState( bInitialState )
{
    if( m_bAutoReset && bInitialState || m_bAutoReset )
			m_mtx.Lock();
}

CEvent::~CEvent()
{
}

void CEvent::Set()
{
    CLock lock( m_lockmtx );
    m_mtx.Unlock();
}

void CEvent::Reset()
{
    CLock lock( m_lockmtx );
    m_mtx.TryLock();
}

void CEvent::Wait()
{
    CLock lock( m_lockmtx );
    m_mtx.Lock();
	if( !m_bAutoReset )
		Set();
}

bool CEvent::Wait( int iTimeout )
{
    CLock lock( m_lockmtx );
    bool bRes = m_mtx.Lock( iTimeout );
    if( bRes && !m_bAutoReset )
		Set();
    return bRes;
}

bool CEvent::TryWait()
{
    CLock lock( m_lockmtx );
    bool bRes = m_mtx.TryLock();
	if( bRes && !m_bAutoReset )
		Set();
    return bRes;
}

