#ifndef _CDBPROVFACTORY
#define _CDBPROVFACTORY

#include "CDBProvider.h"
#include "Singleton.hpp"

class CDbProviderFactory
{
public:
	CDbProviderFactory(void);
	virtual ~CDbProviderFactory(void);

	CDBProvider* GetProviderInstance();

private:

	CDbProviderFactory( const CDbProviderFactory& );
	CDbProviderFactory& operator=( const CDbProviderFactory& );

	HMODULE m_hLib;

	CDBProvider* m_pDbInstance;
};

typedef CSingleton< CDbProviderFactory > DbProviderFactory;

#endif
