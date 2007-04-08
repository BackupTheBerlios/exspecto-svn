#include "precomp.h"
#include "dbproviderfactory.h"


CDbProviderFactory::CDbProviderFactory(void):m_pDbInstance( NULL )
{
	Log::instance().Trace( 10, "CDbProviderFactory::CDbProviderFactor: Загрузка провайдера БД" );
	std::string strLibName;
	Settings::instance().GetParam( DB_PROV_NAME, strLibName );
	if( NULL == ( m_hLib = LoadLibrary( strLibName.c_str() ) ) )
		//TODO:
		throw 1;

	typedef void*(__stdcall*RP)(void);
	RP pFunc;
	if( NULL == ( pFunc = ( (RP)GetProcAddress( m_hLib, "RegisterPlugin" ) ) ) )
		//TODO:
	{
		FreeLibrary( m_hLib );
		throw 1;
	}
	if( NULL == ( m_pDbInstance = (CDBProvider*)pFunc() ) )
		//TODO:
	{
		FreeLibrary( m_hLib );
		throw 1;
	}
		
}

CDbProviderFactory::~CDbProviderFactory(void)
{
	FreeLibrary( m_hLib );
}

CDBProvider* CDbProviderFactory::GetProviderInstance()
{
	return m_pDbInstance;
}
/*
//Инициализируем при старте приложения
namespace{
	const CDBProvider* b = DbProviderFactory::instance().GetProviderInstance(); 
};
*/
