#include "precomp.h"
#include "dbproviderfactory.h"


CDbProviderFactory::CDbProviderFactory(void):m_pDbInstance( NULL )
{
	Log::instance().Trace( 10, "CDbProviderFactory::CDbProviderFactor: Загрузка провайдера БД" );
	std::string strLibName;
	Settings::instance().GetParam( DB_PROV_NAME, strLibName );
	if( NULL == ( m_hLib = LoadLibrary( strLibName.c_str() ) ) )
		throw std::runtime_error( "Не удалось загрузить библиотеку-провайдер БД: " + strLibName );

	typedef void*(__stdcall*RP)(void);
	RP pFunc;
	if( NULL == ( pFunc = ( (RP)GetProcAddress( m_hLib, "RegisterPlugin" ) ) ) )
	{
		FreeLibrary( m_hLib );
		throw std::runtime_error( "Не удалось получить адрес функции RegisterPlugin в библиотеке-провайдере БД: " +strLibName );
	}
	if( NULL == ( m_pDbInstance = (CDBProvider*)pFunc() ) )
	{
		FreeLibrary( m_hLib );
		throw std::runtime_error( "Не удалось получить класс провайдера БД из библиотеки: " +strLibName );
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

