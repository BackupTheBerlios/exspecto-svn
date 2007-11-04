//------------------------------------------------------------------------------------//
//���� ���� �������� ������ ������� Exspecto 2006�.
//Module: Plugin load strategy
//Author: Parshin Dmitry
//Description: �����, ����������� ��������� �������� �������� � ����� Container
//-------------------------------------------------------------------------------------//
#include "precomp.h"
#include "PluginContainer.h"

PluginContainer::PluginContainer()
{
	WIN32_FIND_DATA FindData;
	HANDLE hFindFile;
	HINSTANCE hLib;
	int iScannersCount = 0;
	std::string strPluginPath = PLUGIN_PATH;
	std::string strPluginFileName;
	ScanFunc pScanFunc;
	GetProtoNameFunc pGetProtoName;
	strPluginPath += "\\*.dll";
	Log::instance().Trace( 90, "PluginLoadStrategy: ��������� �������, ���� ��� ������: %s", strPluginPath.c_str() );
	//������� ��� dll � ����� � plugin-���
	if( INVALID_HANDLE_VALUE == ( hFindFile = ::FindFirstFile( strPluginPath.c_str(), &FindData ) ) )
		throw PluginLoadErr( "PluginContainer:�� ������� �� ������ �������" );
	do
	{
		strPluginFileName = PLUGIN_PATH;
		strPluginFileName += "\\";
		strPluginFileName += FindData.cFileName;
		SmartPtr<CSharedLib> pLib = SmartPtr<CSharedLib>( new CSharedLib() );
		if( !pLib->Load( strPluginFileName ) )
		{
			Log::instance().Trace( 50, "PluginContainer: %s �� �������� �����������", FindData.cFileName );
			continue;
		}
		if( NULL == ( pScanFunc = ( ScanFunc )pLib->GetSymbol( "Scan" ) ) )
		{
			Log::instance().Trace( 50, "PluginContainer: �� ������� �������� ����� ������� Scan �� ���������� %s", FindData.cFileName );
			continue;
		}
 		if( NULL == ( pGetProtoName = ( GetProtoNameFunc )pLib->GetSymbol( "GetProtocolName" ) ) )
 		{
			Log::instance().Trace( 50, "PluginContainer: �� ������� �������� ����� ������� GetProtocolName �� ���������� %s", FindData.cFileName );
			continue;
 		}
		//��������� ������ m_mapLibraries ��� ���������� ���������� �������� dll
		m_vecLibraries.push_back( pLib );

		m_mapScanners[ pGetProtoName() ] = pScanFunc;
		Log::instance().Trace( 90, "PluginContainer: ��������� ���������� %s � �������� %s", FindData.cFileName, pGetProtoName() );
		iScannersCount++;
	}while( ::FindNextFile( hFindFile, &FindData ) );
	FindClose( hFindFile );
	if( 0 == iScannersCount )
		throw PluginLoadErr( "�� ������� �� ������ �������" );
	Log::instance().Trace( 90, "PluginContainer: ����� ��������� ��������: %d", iScannersCount );
}

PluginContainer::~PluginContainer()
{
	Log::instance().Trace( 90, "PluginContainer: �����������" );
}
