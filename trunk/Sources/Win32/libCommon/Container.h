#ifndef CONTAINER_H_
#define CONTAINER_H_

#include <vector>

template< class T, class FindStrategy >class Container
{
public:

	typedef typename std::vector< T >::iterator iterator;

	Container(){};
	virtual ~Container(){};
	
	iterator begin();
	iterator end();
	int count();
	
	
	
private:
	
	std::vector< T > m_vecStorage;
};

#endif /*CONTAINER_H_*/
