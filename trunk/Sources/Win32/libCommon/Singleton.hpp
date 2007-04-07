#ifndef SINGLETON
#define SINGLETON
#include "CriticalSection.hpp"

template< class T >
class CSingleton
{
public:
	
	static T& instance()
	{
		static CCriticalSection cs;
		CLock lock( cs );
		static T instance;
		return instance;
	}
private:

	CSingleton();
};

#endif
