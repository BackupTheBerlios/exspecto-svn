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
		if( NULL == ( hLib = ::LoadLibraryA( strPluginFileName.c_str() ) ) )
		{
			Log::instance().Trace( 50, "%d", GetLastError() );
			Log::instance().Trace( 50, "PluginContainer: %s �� �������� �����������", FindData.cFileName );
			continue;
		}
		if( NULL == ( pScanFunc = ( ScanFunc )::GetProcAddress( hLib, "Scan" ) ) )
		{
			Log::instance().Trace( 50, "PluginContainer: �� ������� �������� ����� ������� Scan �� ���������� %s", FindData.cFileName ); 
			::FreeLibrary( hLib );
			continue;
		}
 		if( NULL == ( pGetProtoName = ( GetProtoNameFunc )::GetProcAddress( hLib, "GetProtocolName" ) ) )
 		{
			Log::instance().Trace( 50, "PluginContainer: �� ������� �������� ����� ������� GetProtocolName �� ���������� %s", FindData.cFileName ); 
			::FreeLibrary( hLib );
			continue;
 		} 
		//��������� ������ m_mapLibraries ��� ���������� ���������� �������� dll
		m_vecLibraries.push_back( hLib );

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
	for( std::vector< HMODULE >::iterator It = m_vecLibraries.begin(); It != m_vecLibraries.end(); It++ )
		::FreeLibrary( *It );
}
