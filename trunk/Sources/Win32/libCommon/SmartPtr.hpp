#ifndef CSMARTPTR_H_
#define CSMARTPTR_H_
#include <map>
#include "CriticalSection.hpp"
#include "CLog.h"


template< class T >
class AllocNew
{
public:
	
	static void Destroy( T* pointer )
	{
		delete pointer;
	};
	
};

template< class T >
class AllocNewArray
{
public:
	
	static void Destroy( T* pointer )
	{
		delete[] pointer;
	};
};

template< class T >
class AllocMalloc
{
public:	
	static void Destroy( T* pointer )
	{
		free( pointer );
	};
};

template< class T, class AllocationPolicy = AllocNew< T > >
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
	
	SmartPtr( const SmartPtr< T, AllocationPolicy >& SmartPointer )
	{
		m_pPointer = SmartPointer.m_pPointer;
		m_csMap.Enter();
		m_mapRefs[ m_pPointer ]++;
		m_csMap.Leave();
	}
	
	bool operator==( const SmartPtr< T, AllocationPolicy >& SmartPointer )
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
				AllocationPolicy::Destroy( m_pPointer );
			m_csMap.Leave();
		}
		m_pPointer = pPointer;
	}
	
	SmartPtr& operator=( const SmartPtr< T, AllocationPolicy >& SmartPointer )
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
					AllocationPolicy::Destroy( m_pPointer );
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
				AllocationPolicy::Destroy( m_pPointer );
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
				AllocationPolicy::Destroy( m_pPointer );
				m_pPointer = NULL;
			}
			m_csMap.Leave();
		}		
	}
	
	void Realloc( int iNewSize )
	{

		T* pTmp = m_pPointer;
		AllocationPolicy::Realloc( m_pPointer, iNewSize );
		if( pTmp != m_pPointer )
		{
			m_mapRefs[ m_pPointer ] = m_mapRefs[ pTmp ];
			m_mapRefs[ pTmp ] = 0;
		}

	}
	
	T& operator*()
	{
		return *m_pPointer;
	}
	
private:
	
	T* m_pPointer;
	
	static std::map< T*, int > m_mapRefs;

	static CCriticalSection m_csMap;
};

template< class T, class AllocationPolicy > std::map< T*, int > SmartPtr< T, AllocationPolicy >::m_mapRefs;

template< class T, class AllocationPolicy > CCriticalSection SmartPtr< T, AllocationPolicy >::m_csMap;

#endif /*CSMARTPTR_H_*/
