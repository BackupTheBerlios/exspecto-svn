#ifndef CSMARTPTR_H_
#define CSMARTPTR_H_
#include <map>
#include "CriticalSection.hpp"
#include "CLog.h"


template< class T >
class SmartPtr
{
public:
	SmartPtr():m_pPointer( NULL ){};
	
	SmartPtr( T* pPointer ):m_pPointer( pPointer )
	{
		m_csMap.Enter();
		m_mapRefs[ pPointer ]++;
		m_csMap.Leave();
	}
	
	SmartPtr( const SmartPtr< T >& SmartPointer )
	{
		m_pPointer = SmartPointer.m_pPointer;
		m_csMap.Enter();
		m_mapRefs[ m_pPointer ]++;
		m_csMap.Leave();
	}
	
	bool operator==( const SmartPtr< T >& SmartPointer )
	{
		return m_pPointer==SmartPointer.m_pPointer;
	}

	SmartPtr& operator=( T* pPointer )
	{
		if( NULL != m_pPointer )
		{
			m_csMap.Enter();
			m_mapRefs[ m_pPointer ]--;
			//TODO:
			if( m_mapRefs[ m_pPointer ] < 0 )
				Log::instance().Trace( 0, "SmartPtr: MEMORY LEAK" );
			if( 0 == m_mapRefs[ m_pPointer ] )
				delete m_pPointer;
			m_csMap.Leave();
		}
		m_pPointer = pPointer;
	}
	
	SmartPtr& operator=( const SmartPtr< T >& SmartPointer )
	{
		if( this != &SmartPointer && m_pPointer != SmartPointer.m_pPointer )
		{
			if( NULL != m_pPointer )
			{
				m_csMap.Enter();
				m_mapRefs[ m_pPointer ]--;
				//TODO:
				if( m_mapRefs[ m_pPointer ] < 0 )
					Log::instance().Trace( 0, "SmartPtr: MEMORY LEAK" );

				if( 0 == m_mapRefs[ m_pPointer ] )
					delete m_pPointer;
				m_csMap.Leave();
			}
			m_pPointer = SmartPointer.m_pPointer;
			m_csMap.Enter();
			m_mapRefs[ m_pPointer ]++;
			m_csMap.Leave();
		}
		return *this;
	}
	
	T* operator->()const
	{
		return m_pPointer;
	}
	 
	virtual ~SmartPtr()
	{
		if( NULL != m_pPointer )
		{
			m_csMap.Enter();
			m_mapRefs[ m_pPointer ]--;
			//TODO:
			if( m_mapRefs[ m_pPointer ] < 0 )
				Log::instance().Trace( 0, "SmartPtr: MEMORY LEAK" );

			if( 0 == m_mapRefs[ m_pPointer ] )
				delete m_pPointer;
			m_csMap.Leave();
		}
	}
	
	T* get()
	{
		return m_pPointer;
	}

	void Release()
	{
		if( NULL != m_pPointer )
		{
			m_csMap.Enter();
			m_mapRefs[ m_pPointer ]--;
			//TODO:
			if( m_mapRefs[ m_pPointer ] < 0 )
				Log::instance().Trace( 0, "SmartPtr: MEMORY LEAK" );

			if( 0 == m_mapRefs[ m_pPointer ] )
			{
				delete m_pPointer;
				m_pPointer = NULL;
			}
			m_csMap.Leave();
		}		
	}
	
private:
	
	T* m_pPointer;
	
	static std::map< T*, int > m_mapRefs;

	static CCriticalSection m_csMap;
};

template< class T > std::map< T*, int > SmartPtr< T >::m_mapRefs;

template< class T > CCriticalSection SmartPtr< T >::m_csMap;

#endif /*CSMARTPTR_H_*/
