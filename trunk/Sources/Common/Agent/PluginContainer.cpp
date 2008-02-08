//------------------------------------------------------------------------------------//
//���͜�����ޜ��� ������М��ٜ��� ������Ҝ��ۜ�����՜��������� ������М������������ ���ߜ������ޜ��՜��ڜ������ Exspecto 2006����.
//Module: Plugin load strategy
//Author: Parshin Dmitry
//Description: �������ۜ��М������, ���ޜ��ߜ��؜��������Ҝ��М��������؜��� �������������М�����՜��Ӝ��؜��� ���ל��М��Ӝ���������ל��ڜ��� ���ߜ��ۜ��М��Ӝ��؜��ݜ��ޜ��� ���� ���ڜ��ۜ��М������ Container
//-------------------------------------------------------------------------------------//
#include "precomp.h"
#include "PluginContainer.h"
#include "FileSearch.h"

PluginContainer::PluginContainer()
{
	int iScannersCount = 0;
	std::string strPluginPath = PLUGIN_PATH;
	std::string strPluginFileName;
	ScanFunc pScanFunc;
	GetProtoNameFunc pGetProtoName;
	strPluginPath += "\\*.dll";
	Log::instance().Trace( 90, "PluginLoadStrategy: �������М��Ӝ���������֜��М��՜��� ���ߜ��ۜ��М��Ӝ��؜��ݜ���, ���ߜ��������� ���Ԝ��ۜ��� ���ߜ��ޜ��؜�����ڜ���: %s", strPluginPath.c_str() );
	//�������М�����ޜ��Ԝ��؜��� ���Ҝ������ dll ���� ���ߜ��М��ߜ��ڜ��� ���� plugin-���М��ܜ���
	FileSearch fs;
	std::vector<std::string> vecPluginFiles = fs.GetFiles( strPluginPath );
	for( std::vector<std::string>::iterator It = vecPluginFiles.begin(); It != vecPluginFiles.end(); It++ )
	{
		strPluginFileName = PLUGIN_PATH;
		strPluginFileName += "\\";
		strPluginFileName += *It;
		SmartPtr<CSharedLib> pLib = SmartPtr<CSharedLib>( new CSharedLib() );
		if( !pLib->Load( strPluginFileName ) )
		{
			Log::instance().Trace( 50, "PluginContainer: %s ���ݜ��� ������Ҝ��ۜ�����՜��������� ���ќ��؜��ќ��ۜ��؜��ޜ�����՜��ڜ��ޜ���", It->c_str() );
			continue;
		}
		if( NULL == ( pScanFunc = ( ScanFunc )pLib->GetSymbol( "Scan" ) ) )
		{
			Log::instance().Trace( 50, "PluginContainer: ���ݜ��� ������Ԝ��М��ۜ��ޜ������ ���ߜ��ޜ��ۜ��������؜������ ���М��Ԝ������՜��� ���������ݜ��ڜ�����؜��� Scan ���؜��� ���ќ��؜��ќ��ۜ��؜��ޜ�����՜��ڜ��� %s", It->c_str() );
			continue;
		}
 		if( NULL == ( pGetProtoName = ( GetProtoNameFunc )pLib->GetSymbol( "GetProtocolName" ) ) )
 		{
			Log::instance().Trace( 50, "PluginContainer: ���ݜ��� ������Ԝ��М��ۜ��ޜ������ ���ߜ��ޜ��ۜ��������؜������ ���М��Ԝ������՜��� ���������ݜ��ڜ�����؜��� GetProtocolName ���؜��� ���ќ��؜��ќ��ۜ��؜��ޜ�����՜��ڜ��� %s", It->c_str() );
			continue;
 		}
		//�������М��ߜ��ޜ��ۜ��ݜ�����՜��� ���ܜ��М��������؜��� m_mapLibraries ���Ԝ��ۜ��� ���Ԝ��М��ۜ�����ݜ��՜��ٜ�����՜��� ���ڜ��ޜ����������՜��ڜ�����ݜ��ޜ��� ���Ҝ�����Ӝ���������ל��ڜ��� dll
		m_vecLibraries.push_back( pLib );

		m_mapScanners[ pGetProtoName() ] = pScanFunc;
		Log::instance().Trace( 90, "PluginContainer: �������М��Ӝ���������֜��М��՜��� ���ќ��؜��ќ��ۜ��؜��ޜ�����՜��ڜ��� %s ���� ���ߜ��ۜ��М��Ӝ��؜��ݜ��ޜ��� %s", It->c_str(), pGetProtoName() );
		iScannersCount++;
	}
	if( 0 == iScannersCount )
		throw PluginLoadErr( "PluginContainer:�������� ���ݜ��М��ٜ��Ԝ��՜��ݜ��� ���ݜ��� ���ޜ��Ԝ��ݜ��ޜ��Ӝ��� ���ߜ��ۜ��М��Ӝ��؜��ݜ���" );
	Log::instance().Trace( 90, "PluginContainer: ���Ҝ�����՜��Ӝ��� ���ל��М��Ӝ���������֜��՜��ݜ��� ���ߜ��ۜ��М��Ӝ��؜��ݜ��ޜ���: %d", iScannersCount );
}

PluginContainer::~PluginContainer()
{
	Log::instance().Trace( 90, "PluginContainer: ���Ü��ݜ��؜��������ޜ��֜��՜��ݜ��؜���" );
}
