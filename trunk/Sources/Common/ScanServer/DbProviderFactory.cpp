#include "precomp.h"
#include "DbProviderFactory.h"


CDbProviderFactory::CDbProviderFactory(void):m_pDbInstance( NULL )
{
	Log::instance().Trace( 10, "CDbProviderFactory::CDbProviderFactor: œô¨·œô¨Ðœô¨Óœô¨àœô¨ãœô¨×œô¨Úœô¨Ð œô¨ßœô¨àœô¨Þœô¨Òœô¨Ðœô¨Ùœô¨Ôœô¨Õœô¨àœô¨Ð œô¨±œô¨´" );
	std::string strLibName;
	Settings::instance().GetParam( DB_PROV_NAME, strLibName );
	m_pLib = SmartPtr<CSharedLib>( new CSharedLib() );
	if( !m_pLib->Load( strLibName.c_str() ) )
		throw std::runtime_error( "œô¨½œô¨Õ œô¨ãœô¨Ôœô¨Ðœô¨Ûœô¨Þœô¨áœô¨ì œô¨×œô¨Ðœô¨Óœô¨àœô¨ãœô¨×œô¨Øœô¨âœô¨ì œô¨Ñœô¨Øœô¨Ñœô¨Ûœô¨Øœô¨Þœô¨âœô¨Õœô¨Úœô¨ã-œô¨ßœô¨àœô¨Þœô¨Òœô¨Ðœô¨Ùœô¨Ôœô¨Õœô¨à œô¨±œô¨´: " + strLibName );

	typedef void*(*RP)(void);
	RP pFunc;
	if( NULL == ( pFunc = ( (RP)m_pLib->GetSymbol( "RegisterPlugin" ) ) ) )
		throw std::runtime_error( "œô¨½œô¨Õ œô¨ãœô¨Ôœô¨Ðœô¨Ûœô¨Þœô¨áœô¨ì œô¨ßœô¨Þœô¨Ûœô¨ãœô¨çœô¨Øœô¨âœô¨ì œô¨Ðœô¨Ôœô¨àœô¨Õœô¨á œô¨äœô¨ãœô¨Ýœô¨Úœô¨æœô¨Øœô¨Ø RegisterPlugin œô¨Ò œô¨Ñœô¨Øœô¨Ñœô¨Ûœô¨Øœô¨Þœô¨âœô¨Õœô¨Úœô¨Õ-œô¨ßœô¨àœô¨Þœô¨Òœô¨Ðœô¨Ùœô¨Ôœô¨Õœô¨àœô¨Õ œô¨±œô¨´: " +strLibName );
	if( NULL == ( m_pDbInstance = (CDBProvider*)pFunc() ) )
		throw std::runtime_error( "œô¨½œô¨Õ œô¨ãœô¨Ôœô¨Ðœô¨Ûœô¨Þœô¨áœô¨ì œô¨ßœô¨Þœô¨Ûœô¨ãœô¨çœô¨Øœô¨âœô¨ì œô¨Úœô¨Ûœô¨Ðœô¨áœô¨á œô¨ßœô¨àœô¨Þœô¨Òœô¨Ðœô¨Ùœô¨Ôœô¨Õœô¨àœô¨Ð œô¨±œô¨´ œô¨Øœô¨× œô¨Ñœô¨Øœô¨Ñœô¨Ûœô¨Øœô¨Þœô¨âœô¨Õœô¨Úœô¨Ø: " +strLibName );
}

CDBProvider* CDbProviderFactory::GetProviderInstance()
{
	return m_pDbInstance;
}

