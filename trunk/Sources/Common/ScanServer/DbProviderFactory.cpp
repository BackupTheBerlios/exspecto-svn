#include "precomp.h"
#include "DbProviderFactory.h"


CDbProviderFactory::CDbProviderFactory(void):m_pDbInstance( NULL )
{
	Log::instance().Trace( 10, "CDbProviderFactory::CDbProviderFactor: �������М��Ӝ���������ל��ڜ��� ���ߜ������ޜ��Ҝ��М��ٜ��Ԝ��՜������� ��������" );
	std::string strLibName;
	Settings::instance().GetParam( DB_PROV_NAME, strLibName );
	m_pLib = SmartPtr<CSharedLib>( new CSharedLib() );
	if( !m_pLib->Load( strLibName.c_str() ) )
		throw std::runtime_error( "�������� ������Ԝ��М��ۜ��ޜ������ ���ל��М��Ӝ���������ל��؜������ ���ќ��؜��ќ��ۜ��؜��ޜ�����՜��ڜ���-���ߜ������ޜ��Ҝ��М��ٜ��Ԝ��՜��� ��������: " + strLibName );

	typedef void*(*RP)(void);
	RP pFunc;
	if( NULL == ( pFunc = ( (RP)m_pLib->GetSymbol( "RegisterPlugin" ) ) ) )
		throw std::runtime_error( "�������� ������Ԝ��М��ۜ��ޜ������ ���ߜ��ޜ��ۜ��������؜������ ���М��Ԝ������՜��� ���������ݜ��ڜ�����؜��� RegisterPlugin ���� ���ќ��؜��ќ��ۜ��؜��ޜ�����՜��ڜ���-���ߜ������ޜ��Ҝ��М��ٜ��Ԝ��՜������� ��������: " +strLibName );
	if( NULL == ( m_pDbInstance = (CDBProvider*)pFunc() ) )
		throw std::runtime_error( "�������� ������Ԝ��М��ۜ��ޜ������ ���ߜ��ޜ��ۜ��������؜������ ���ڜ��ۜ��М������ ���ߜ������ޜ��Ҝ��М��ٜ��Ԝ��՜������� �������� ���؜��� ���ќ��؜��ќ��ۜ��؜��ޜ�����՜��ڜ���: " +strLibName );
}

CDBProvider* CDbProviderFactory::GetProviderInstance()
{
	return m_pDbInstance;
}

