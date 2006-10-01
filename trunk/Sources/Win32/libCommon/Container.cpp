#include "Container.h"

Container< class T, class FindStrategy >::iterator Container< T, FindStrategy >::begin()
{
	return m_vecStorage.begin();
}

Container< class T, class FindStrategy >::iterator Container< T, FindStrategy >::end()
{
	return m_vecStorage.end();
}

template< class T, class FindStrategy >
int Container< T, FindStrategy >::count()
{
	return m_vecStorage.size();
}

