#ifndef CSMARTPTR_H_
#define CSMARTPTR_H_
#include <map>
#include "CriticalSection.hpp"
#include "CLog.h"
#include <assert.h>


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
	friend class SmartPtr;
public:
	SmartPtr():m_pPointerImpl( NULL ){};
	
	SmartPtr( T* pPointer ):m_pPointerImpl( NULL )
	{
		/*
		m_csMap.Enter();
		m_mapRefs[ pPointer ]++;
		m_csMap.Leave();
		*/
		m_pPointerImpl = new Ref;
		m_pPointerImpl->pPointer = pPointer;
		m_pPointerImpl->iRefCount = 1;
		
	}
	
	template< class T1 >
	SmartPtr( SmartPtr< T1 >& SmartPointer ):m_pPointerImpl( NULL )
	{
		/*
		m_pPointer = SmartPointer.get();
		m_csMap.Enter();
		m_mapRefs[ m_pPointer ]++;
		m_csMap.Leave();
		*/
		if( SmartPointer.m_pPointerImpl != NULL )
		{
			m_pPointerImpl = reinterpret_cast<SmartPtr<T>::Ref*>( SmartPointer.m_pPointerImpl );
			m_pPointerImpl->cs.Enter();
			m_pPointerImpl->iRefCount = 1;
			m_pPointerImpl->cs.Leave();
		}
	}
	
	SmartPtr( const SmartPtr< T >& SmartPointer ):m_pPointerImpl( NULL )
	{
		/*
		m_pPointer = SmartPointer.get();
		m_csMap.Enter();
		m_mapRefs[ m_pPointer ]++;
		m_csMap.Leave();
		*/
		if( SmartPointer.m_pPointerImpl != NULL )
		{
			m_pPointerImpl = SmartPointer.m_pPointerImpl;
			m_pPointerImpl->cs.Enter();
			++m_pPointerImpl->iRefCount;
			m_pPointerImpl->cs.Leave();
		}

	}

	template< class T1 >
	bool operator==( const SmartPtr< T1 >& SmartPointer )
	{
		return m_pPointerImpl->pPointer == SmartPointer.m_pPointerImpl->pPointer;
	}

	SmartPtr& operator=( T* pPointer )
	{
/*
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
*/
		if( NULL != m_pPointerImpl )
		{
			m_pPointerImpl->cs.Enter();
			--m_pPointerImpl->iRefCount;
			if( m_pPointerImpl->iRefCount < 0 )
				Log::instance().Trace( 0, "SmartPtr: MEMORY LEAK" );
			
			if( 0 == m_pPointerImpl->iRefCount  )
			{
				m_pPointerImpl->cs.Leave();
				AllocationPolicy::Destroy( m_pPointerImpl->pPointer );
			}else
			{
				m_pPointerImpl->cs.Leave();
				m_pPointerImpl = new Ref;
			}
			m_pPointerImpl->pPointer = pPointer;
			m_pPointerImpl->iRefCount = 1;
		}

	}
	
	template< class T1 >
	SmartPtr& operator=( const SmartPtr< T1 >& SmartPointer )
	{
		/*
		if( this != &SmartPointer && m_pPointer != SmartPointer.get() )
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
			m_pPointer = SmartPointer.get();
			m_csMap.Enter();
			m_mapRefs[ m_pPointer ]++;
			m_csMap.Leave();
		}
		*/
		if( this != &SmartPointer && m_pPointerImpl != SmartPointer.m_pPointerImpl )
		{
			if( NULL != m_pPointerImpl )
			{
				m_pPointerImpl->cs.Enter();
				--m_pPointerImpl->iRefCount;
				assert( m_pPointerImpl->iRefCount >= 0 && "SmartPtr: MEMORY LEAK"  );
				if( 0 == m_pPointerImpl->iRefCount  )
				{
					m_pPointerImpl->cs.Leave();
					AllocationPolicy::Destroy( m_pPointerImpl->pPointer );
					delete m_pPointerImpl;
				}else
					m_pPointerImpl->cs.Leave();
			}
			m_pPointerImpl = SmartPointer.m_pPointerImpl;
			m_pPointerImpl->cs.Enter();
			++m_pPointerImpl->iRefCount;
			m_pPointerImpl->cs.Leave();
		}
		return *this;
	}

	SmartPtr& operator=( const SmartPtr< T >& SmartPointer )
	{
		/*
		if( this != &SmartPointer && m_pPointer != SmartPointer.get() )
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
			m_pPointer = SmartPointer.get();
			m_csMap.Enter();
			m_mapRefs[ m_pPointer ]++;
			m_csMap.Leave();
		}
		*/
		if( this != &SmartPointer && m_pPointerImpl != SmartPointer.m_pPointerImpl )
		{
			if( NULL != m_pPointerImpl )
			{
				m_pPointerImpl->cs.Enter();
				--m_pPointerImpl->iRefCount;
				assert( m_pPointerImpl->iRefCount >= 0 && "SmartPtr: MEMORY LEAK"  );
				if( 0 == m_pPointerImpl->iRefCount  )
				{
					m_pPointerImpl->cs.Leave();
					AllocationPolicy::Destroy( m_pPointerImpl->pPointer );
					delete m_pPointerImpl;
				}else
					m_pPointerImpl->cs.Leave();
			}
			m_pPointerImpl = SmartPointer.m_pPointerImpl;
			m_pPointerImpl->cs.Enter();
			++m_pPointerImpl->iRefCount;
			m_pPointerImpl->cs.Leave();
		}
		return *this;
	}
	
	T* operator->()const
	{
		return m_pPointerImpl->pPointer;
	}
	 
	virtual ~SmartPtr()
	{
		/*
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
		*/
		if( NULL != m_pPointerImpl )
		{
			m_pPointerImpl->cs.Enter();
			--m_pPointerImpl->iRefCount;
			assert( m_pPointerImpl->iRefCount >= 0 && "SmartPtr: MEMORY LEAK"  );
			if( 0 == m_pPointerImpl->iRefCount  )
			{
				m_pPointerImpl->cs.Leave();
				AllocationPolicy::Destroy( m_pPointerImpl->pPointer );
				delete m_pPointerImpl;
			}else
				m_pPointerImpl->cs.Leave();
		}
	}
	
	T* get()const
	{
		if( m_pPointerImpl )
			return m_pPointerImpl->pPointer;
		else
			return NULL;
	}

	void Release()
	{
		/*
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
		*/
		if( NULL != m_pPointerImpl )
		{
			m_pPointerImpl->cs.Enter();
			--m_pPointerImpl->iRefCount;
			assert( m_pPointerImpl->iRefCount >= 0 && "SmartPtr: MEMORY LEAK"  );
			if( 0 == m_pPointerImpl->iRefCount  )
			{
				m_pPointerImpl->cs.Leave();
				AllocationPolicy::Destroy( m_pPointerImpl->pPointer );
				delete m_pPointerImpl;
			}else
				m_pPointerImpl->cs.Leave();
			m_pPointerImpl = NULL;
		}
	}
/*	
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
*/	
	T& operator*()
	{
		return *m_pPointerImpl->pPointer;
	}

	
private:

	struct Ref{
		CCriticalSection cs;
		int iRefCount;
		T* pPointer;
	};
/*	T* m_pPointer;
	
	static std::map< T*, int > m_mapRefs;

	static CCriticalSection m_csMap;
*/

	Ref* m_pPointerImpl;
};

//template< class T, class AllocationPolicy > std::map< T*, int > SmartPtr< T, AllocationPolicy >::m_mapRefs;

//template< class T, class AllocationPolicy > CCriticalSection SmartPtr< T, AllocationPolicy >::m_csMap;

#endif /*CSMARTPTR_H_*/
