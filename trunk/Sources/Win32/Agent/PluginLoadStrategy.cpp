//------------------------------------------------------------------------------------//
//���� ���� �������� ������ ������� Exspecto 2006�.
//Module: Plugin load strategy
//Author: Parshin Dmitry
//Description: �����, ����������� ��������� �������� �������� � ����� Container
//-------------------------------------------------------------------------------------//
#include "PluginLoadStrategy.h"
#include "windows.h"

PluginLoadStrategy::PluginLoadStrategy( std::vector< CScanner* >& vecStorage )
{
	WIN32_FIND_DATA FindData;
	HANDLE hFindFile;
	HINSTANCE hLib;
	CScanner* pScanner;
	fnGetScannerFunc fnGetScanner;
	fnReleaseScannerFunc fnReleaseScanner;
	int iScannersCount = 0;
	std::string strPluginPath;// = PLUGIN_PATH;
	strPluginPath += "*.dll";
	//������� ��� dll � ����� � plugin-���
	if( INVALID_HANDLE_VALUE == ( hFindFile = ::FindFirstFile( strPluginPath.c_str(), &FindData ) ) )
		return;
	do
	{ 
		if( NULL == ( hLib = ::LoadLibraryA( FindData.cFileName ) )
			|| NULL == ( fnGetScanner = ( fnGetScannerFunc )::GetProcAddress( hLib, "GetScanner" ) )
		 	|| NULL == ( fnReleaseScanner = ( fnReleaseScannerFunc )::GetProcAddress( hLib, "ReleaseScanner" )) 
		 	|| NULL == ( pScanner = fnGetScanner() ) )
		{
			::FreeLibrary( hLib );
			continue;
		}
		//��������� ������ m_mapLibraries ��� ���������� ���������� �������� dll � ��������
		//���������������� ������� CScanner
		m_mapLibraries[ hLib ] = fnReleaseScanner;

		vecStorage.push_back( pScanner );
		iScannersCount++;
	}while( ::FindNextFile( hFindFile, &FindData ) );
}

PluginLoadStrategy::~PluginLoadStrategy()
{
	for( LibrariesMapType::iterator It = m_mapLibraries.begin(); It != m_mapLibraries.end(); It++ )
	{
		It->second();
		::FreeLibrary( It->first );
	}
}
