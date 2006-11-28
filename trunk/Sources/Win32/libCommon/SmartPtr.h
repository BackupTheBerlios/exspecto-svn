#ifndef CSMARTPTR_H_
#define CSMARTPTR_H_
#include <map>


template< class T >
class SmartPtr
{
public:
	SmartPtr():m_pPointer( NULL ){};
	
	SmartPtr( T* pPointer ):m_pPointer( pPointer )
	{
		m_mapRefs[ pPointer ]++;
	}
	
	SmartPtr( const SmartPtr< T >& SmartPointer )
	{
		m_pPointer = SmartPointer.m_pPointer;
		m_mapRefs[ m_pPointer ]++;
	}
	
	SmartPtr& operator=( T* pPointer )
	{
		if( NULL != m_pPointer )
			m_mapRefs[ m_pPointer ]--;
		m_pPointer = pPointer;
	}
	
	SmartPtr& operator=( const SmartPtr< T >& SmartPointer )
	{
		if( this != &SmartPointer )
		{
			if( NULL != m_pPointer )
				m_mapRefs[ m_pPointer ]--;
			if( 0 == m_mapRefs[ m_pPointer ] )
				delete m_pPointer;
			m_pPointer = SmartPointer.m_pPointer;
		}
		return *this;
	}
	
	T* operator->()
	{
		return m_pPointer;
	}
	 
	virtual ~SmartPtr()
	{
		if( NULL != m_pPointer )
			m_mapRefs[ m_pPointer ]--;
		if( 0 == m_mapRefs[ m_pPointer ] )
			delete m_pPointer;
	}
	
	T* get()
	{
		return m_pPointer;
	}

private:
	
	T* m_pPointer;
	
	static std::map< void*, int > m_mapRefs;
};

#endif /*CSMARTPTR_H_*/
