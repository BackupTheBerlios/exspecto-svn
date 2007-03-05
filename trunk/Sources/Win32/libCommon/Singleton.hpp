#ifndef SINGLETON
#define SINGLETON

template< class T >
class CSingleton
{
public:
	
	static T& Instance()
	{
		static T instance;
		return instance;
	}

};

#endif