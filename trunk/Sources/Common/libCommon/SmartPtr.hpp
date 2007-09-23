#ifndef CSMARTPTR_H_
#define CSMARTPTR_H_
#include <map>
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
			m_pPointerImpl->mt.lock();
			++m_pPointerImpl->iRefCount;
			m_pPointerImpl->mt.unlock();
		}
	}


	SmartPtr( const SmartPtr< T, AllocationPolicy >& SmartPointer ):m_pPointerImpl( NULL )
	{
		if( SmartPointer.m_pPointerImpl != NULL )
		{
			m_pPointerImpl = SmartPointer.m_pPointerImpl;
			m_pPointerImpl->mt.lock();
			++m_pPointerImpl->iRefCount;
			m_pPointerImpl->mt.unlock();
		}

	}


	template< class T1 >
	bool operator==( const SmartPtr< T1, AllocationPolicy >& SmartPointer )
	{
		return m_pPointerImpl->pPointer == SmartPointer.m_pPointerImpl->pPointer;
	}

	SmartPtr& operator=( T* pPointer )
	{
		if( NULL != m_pPointerImpl )
		{
			m_pPointerImpl->mt.lock();
			--m_pPointerImpl->iRefCount;
			assert( m_pPointerImpl->iRefCount >= 0 && "SmartPtr: MEMORY LEAK"  );

			if( 0 == m_pPointerImpl->iRefCount  )
			{
				m_pPointerImpl->mt.unlock();
				AllocationPolicy::Destroy( m_pPointerImpl->pPointer );
			}else
			{
				m_pPointerImpl->mt.unlock();
				m_pPointerImpl = new Ref;
			}
		}else
			m_pPointerImpl = new Ref;
		m_pPointerImpl->pPointer = pPointer;
		m_pPointerImpl->iRefCount = 1;


		return *this;
	}

	template< class T1, class T2 >
	SmartPtr& operator=( const SmartPtr< T1, T2 >& SmartPointer )
	{
		if( ( this != &SmartPointer ) && ( m_pPointerImpl != SmartPointer.m_pPointerImpl ) )
		{
			if( NULL != m_pPointerImpl )
			{
				m_pPointerImpl->mt.lock();
				--m_pPointerImpl->iRefCount;
				assert( m_pPointerImpl->iRefCount >= 0 && "SmartPtr: MEMORY LEAK"  );
				if( 0 == m_pPointerImpl->iRefCount  )
				{
					m_pPointerImpl->mt.unlock();
					AllocationPolicy::Destroy( m_pPointerImpl->pPointer );
					delete m_pPointerImpl;
					m_pPointerImpl = NULL;
				}else
					m_pPointerImpl->mt.unlock();
			}
			if( NULL != ( m_pPointerImpl = SmartPointer.m_pPointerImpl ) )
			{
				m_pPointerImpl->mt.lock();
				++m_pPointerImpl->iRefCount;
				m_pPointerImpl->mt.unlock();
			}
		}
		return *this;
	}

	SmartPtr& operator=( const SmartPtr< T, AllocationPolicy >& SmartPointer )
	{
		if( ( this != &SmartPointer ) && ( m_pPointerImpl != SmartPointer.m_pPointerImpl ) )
		{
			if( NULL != m_pPointerImpl )
			{
				m_pPointerImpl->mt.lock();
				--m_pPointerImpl->iRefCount;
				assert( m_pPointerImpl->iRefCount >= 0 && "SmartPtr: MEMORY LEAK"  );
				if( 0 == m_pPointerImpl->iRefCount  )
				{
					m_pPointerImpl->mt.unlock();
					AllocationPolicy::Destroy( m_pPointerImpl->pPointer );
					delete m_pPointerImpl;
					m_pPointerImpl = NULL;
				}else
					m_pPointerImpl->mt.unlock();
			}
			if( NULL != ( m_pPointerImpl = SmartPointer.m_pPointerImpl ) )
			{
				m_pPointerImpl->mt.lock();
				++m_pPointerImpl->iRefCount;
				m_pPointerImpl->mt.unlock();
			}
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
			m_pPointerImpl->mt.lock();
			--m_pPointerImpl->iRefCount;
			assert( m_pPointerImpl->iRefCount >= 0 && "SmartPtr: MEMORY LEAK"  );
			if( 0 == m_pPointerImpl->iRefCount  )
			{
				m_pPointerImpl->mt.unlock();
				AllocationPolicy::Destroy( m_pPointerImpl->pPointer );
				delete m_pPointerImpl;
				m_pPointerImpl = NULL;
			}else
				m_pPointerImpl->mt.unlock();
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
			m_pPointerImpl->mt.lock();
			--m_pPointerImpl->iRefCount;
			assert( m_pPointerImpl->iRefCount >= 0 && "SmartPtr: MEMORY LEAK"  );
			if( 0 == m_pPointerImpl->iRefCount  )
			{
				m_pPointerImpl->mt.unlock();
				AllocationPolicy::Destroy( m_pPointerImpl->pPointer );
				delete m_pPointerImpl;
			}else
				m_pPointerImpl->mt.unlock();
			m_pPointerImpl = NULL;
		}
	}

    T& operator*()
	{
		return *m_pPointerImpl->pPointer;
	}


private:

	//TODO:Можно избавиться от mt, используя Interlocked-функции
	struct Ref{
		pt::mutex mt;
		int iRefCount;
		T* pPointer;
	};

	Ref* m_pPointerImpl;
};


#endif /*CSMARTPTR_H_*/
