#ifndef CONTAINER_H_
#define CONTAINER_H_

#include <vector>

/*class LoadStrategy
{
public:
	virtual void Load() = 0;
};
*/
template< class T, class LoadStrategy >
class Container
{
public:

	typedef typename std::vector< T >::iterator iterator;

	Container();
	virtual ~Container();
	
	iterator begin();
	iterator end();
	int count();
	
	
	
private:
	
//	LoadStrategy m_Loader;
		
	std::vector< T > m_vecStorage;
};

#endif /*CONTAINER_H_*/
