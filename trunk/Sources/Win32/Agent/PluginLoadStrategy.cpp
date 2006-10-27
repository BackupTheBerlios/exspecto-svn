//------------------------------------------------------------------------------------//
//���� ���� �������� ������ ������� Exspecto 2006�.
//Module: Plugin load strategy
//Author: Parshin Dmitry
//Description: �����, ����������� ��������� �������� �������� � ����� Container
//-------------------------------------------------------------------------------------//
#include "PluginLoadStrategy.h"

PluginLoadStrategy::PluginLoadStrategy( std::vector< CScanner* >& vecStorage )throw( PluginLoadErr )
{
	WIN32_FIND_DATA FindData;
	HANDLE hFindFile;
	HINSTANCE hLib;
	CScanner* pScanner;
	fnGetScannerFunc fnGetScanner;
	fnReleaseScannerFunc fnReleaseScanner;
	int iScannersCount = 0;
	std::string strPluginPath = PLUGIN_PATH;
	std::string strPluginFileName;
	
	strPluginPath += "\\*.dll";
	log.Trace( 90, "PluginLoadStrategy: ��������� �������, ���� ��� ������: %s", strPluginPath.c_str() ); 
	//������� ��� dll � ����� � plugin-���
	if( INVALID_HANDLE_VALUE == ( hFindFile = ::FindFirstFile( strPluginPath.c_str(), &FindData ) ) )
		throw PluginLoadErr( "�� ������� �� ������ �������" );
	do
	{ 
		strPluginFileName = PLUGIN_PATH;
		strPluginFileName += "\\";
		strPluginFileName += FindData.cFileName;
		if( NULL == ( hLib = ::LoadLibraryA( strPluginFileName.c_str() ) ) )
		{
			log.Trace( 50, "%d", GetLastError() );
			log.Trace( 50, "PluginLoadStrategy: %s �� �������� �����������", FindData.cFileName );
			continue;
		}
		if( NULL == ( fnGetScanner = ( fnGetScannerFunc )::GetProcAddress( hLib, "GetScanner" ) ) )
		{
			log.Trace( 50, "PluginLoadStrategy: �� ������� �������� ����� ������� GetScanner �� ���������� %s", FindData.cFileName ); 
			::FreeLibrary( hLib );
			continue;
		}
 		if( NULL == ( fnReleaseScanner = ( fnReleaseScannerFunc )::GetProcAddress( hLib, "ReleaseScanner" ) ) )
 		{
			log.Trace( 50, "PluginLoadStrategy: �� ������� �������� ����� ������� ReleaseScanner �� ���������� %s", FindData.cFileName ); 
			::FreeLibrary( hLib );
			continue;
 		} 
		if( NULL == ( pScanner = fnGetScanner() ) )
		{
			log.Trace( 50, "PluginLoadStrategy: ������ ��� ���������� GetScanner �� ���������� %s", FindData.cFileName );			
			::FreeLibrary( hLib );
			continue;
		}
		//��������� ������ m_mapLibraries ��� ���������� ���������� �������� dll � ��������
		//���������������� ������� CScanner
		m_mapLibraries[ hLib ] = fnReleaseScanner;

		vecStorage.push_back( pScanner );
		log.Trace( 90, "PluginLoadStrategy: ��������� ���������� %s � �������� %s", FindData.cFileName, pScanner->GetProtocolName() );
		iScannersCount++;
	}while( ::FindNextFile( hFindFile, &FindData ) );
	log.Trace( 90, "PluginLoadStrategy: ����� ��������� %d ��������", iScannersCount );
}

PluginLoadStrategy::~PluginLoadStrategy()
{
	for( LibrariesMapType::iterator It = m_mapLibraries.begin(); It != m_mapLibraries.end(); It++ )
	{
		It->second();
		::FreeLibrary( It->first );
	}
}
