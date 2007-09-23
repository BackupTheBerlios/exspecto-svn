#ifndef SINGLETON
#define SINGLETON
#include "pasync.h"

template< class T >
class CSingleton
{
public:

	static T& instance()
	{
	    static pt::mutex mutex;
	    pt::scopelock lock( mutex );
		static T instance;
		return instance;
	}
private:

	CSingleton();
};

#endif
