#ifndef SINGLETON
#define SINGLETON

template< class T >
class CSingleton
{
public:
	
	static T& instance()
	{
		static T instance;
		return instance;
	}
private:

	CSingleton();
};

#endif
