#ifndef SEMAPHORE_HPP_
#define SEMAPHORE_HPP_

class CSemaphore
{
public:
	CSemaphore( int iResourceCount = INT_MAX ):m_sem( iResourceCount )
	{};
	
	~CSemaphore()
	{}
	
	void Release()
	{
		m_sem.post();
	}
	
private:

	CSemaphore( const CSemaphore& );
	CSemaphore& operator=( const CSemaphore& );

	pt::semaphore m_sem;
};

#endif /*EVENT_HPP_*/
