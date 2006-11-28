#include "SmartPtr.h"

template< class T > std::map< T*, int > SmartPtr< T >::m_mapRefs;
