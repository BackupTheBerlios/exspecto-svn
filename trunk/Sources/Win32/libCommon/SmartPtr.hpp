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
	
	static void Destroy( T*& pointer )
	{
		delete pointer;
		pointer = NULL;
	};
	
};

template< class T >
class AllocNewArray
{
public:
	
	static void Destroy( T*& pointer )
	{
		delete[] pointer;
		pointer = NULL;
	};
};

template< class T >
class AllocMalloc
{
public:	
	static void Destroy( T*& pointer )
	{
		free( pointer );
		pointer = NULL;
	};
};

template< class T, class AllocationPolicy = AllocNew< T > >
class SmartPtr
{
	template<class,class> friend class SmartPtr;
public:
	SmartPtr():m_pPointerImpl( NULL ){};
	
	SmartPtr( T* pPointer ):m_pPointerImpl( NULL )
	{
		m_pPointerImpl = new Ref;
		m_pPointerImpl->pPointer = pPointer;
		m_pPointerImpl->iRefCount = 1;
		
	}
	
	template< class T1, class T2 >
	SmartPtr( SmartPtr< T1, T2 >& SmartPointer ):m_pPointerImpl( NULL )
	{
		if( SmartPointer.m_pPointerImpl != NULL )
		{
			m_pPointerImpl = reinterpret_cast<Ref*>( SmartPointer.m_pPointerImpl );
			m_pPointerImpl->cs.Enter();
			++m_pPointerImpl->iRefCount;
			m_pPointerImpl->cs.Leave();
		}
	}
	

	SmartPtr( const SmartPtr< T, AllocationPolicy >& SmartPointer ):m_pPointerImpl( NULL )
	{
		if( SmartPointer.m_pPointerImpl != NULL )
		{
			m_pPointerImpl = SmartPointer.m_pPointerImpl;
			m_pPointerImpl->cs.Enter();
			++m_pPointerImpl->iRefCount;
			m_pPointerImpl->cs.Leave();
		}

	}


	template< class T1, class T2 >
	bool operator==( const SmartPtr< T1, AllocationPolicy >& SmartPointer )
	{
		return m_pPointerImpl->pPointer == SmartPointer.m_pPointerImpl->pPointer;
	}

	SmartPtr& operator=( T* pPointer )
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
			}else
			{
				m_pPointerImpl->cs.Leave();
				m_pPointerImpl = new Ref;
			}
			m_pPointerImpl->pPointer = pPointer;
			m_pPointerImpl->iRefCount = 1;
		}

	}
	
	template< class T1, class T2 >
	SmartPtr& operator=( const SmartPtr< T1, T2 >& SmartPointer )
	{
		if( ( this != &SmartPointer ) && ( m_pPointerImpl != SmartPointer.m_pPointerImpl ) )
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
					m_pPointerImpl = NULL;
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

	SmartPtr& operator=( const SmartPtr< T, AllocationPolicy >& SmartPointer )
	{
		if( ( this != &SmartPointer ) && ( m_pPointerImpl != SmartPointer.m_pPointerImpl ) )
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
					m_pPointerImpl = NULL;
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
				m_pPointerImpl = NULL;
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

	//TODO:
	//Можно избавиться от cs, используя Interlocked-функции
	struct Ref{
		CCriticalSection cs;
		int iRefCount;
		T* pPointer;
	};

	Ref* m_pPointerImpl;
};


#endif /*CSMARTPTR_H_*/
