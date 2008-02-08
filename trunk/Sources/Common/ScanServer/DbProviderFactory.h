#ifndef _CDBPROVFACTORY
#define _CDBPROVFACTORY

#include <CDBProvider.h>
#include <Singleton.hpp>
#include <SharedLib.h>
#include <SmartPtr.hpp>

class CDbProviderFactory
{
public:
	CDbProviderFactory(void);
	virtual ~CDbProviderFactory(void){};

	CDBProvider* GetProviderInstance();

private:

	CDbProviderFactory( const CDbProviderFactory& );
	CDbProviderFactory& operator=( const CDbProviderFactory& );

    SmartPtr<CSharedLib> m_pLib;

	CDBProvider* m_pDbInstance;
};

typedef CSingleton< CDbProviderFactory > DbProviderFactory;

#endif
