#ifndef SINGLETON
#define SINGLETON
#include "Mutex.h"

template< class T >
class CSingleton
{
public:

	static T& instance()
	{
	    static CMutex mutex;
	    CLock lock( mutex );
		static T instance;
		return instance;
	}
private:

	CSingleton();
};

#endif
