#include "Container.h"

template< class T, class LoadStrategy >
Container< T, LoadStrategy >::Container()
{
}

template< class T, class LoadStrategy >
Container< T, LoadStrategy >::~Container()
{
}

Container< class T, class LoadStrategy >::iterator Container< T, LoadStrategy >::begin()
{
	return m_vecStorage.begin();
}

Container< class T, class LoadStrategy >::iterator Container< T, LoadStrategy >::end()
{
	return m_vecStorage.end();
}

template< class T, class LoadStrategy >
int Container< T, LoadStrategy >::count()
{
	return m_vecStorage.size();
}

