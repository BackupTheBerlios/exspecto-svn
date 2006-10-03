//------------------------------------------------------------------------------------//
//���� ���� �������� ������ ������� Exspecto 2006�.
//Module: Plugin load strategy
//Author: Parshin Dmitry
//Description: �����, ����������� ��������� �������� �������� � ����� Container
//-------------------------------------------------------------------------------------//
#ifndef PLUGINLOADSTRATEGY_H_
#define PLUGINLOADSTRATEGY_H_

#include "windows.h"
#include "CScanner.h"
#include <map>

class PluginLoadStrategy
{
public:
	PluginLoadStrategy( std::vector< CScanner* >& vecStorage );
	virtual ~PluginLoadStrategy();
	
	//���� �������, ������������ � dll � plugin-��
	typedef CScanner* (*fnGetScannerFunc)();
	typedef void (*fnReleaseScannerFunc)();
	

	typedef std::map< HINSTANCE , fnReleaseScannerFunc > LibrariesMapType;
	//������ ������������ �������� ��������� � ��������������� �� ������� ������� 
	LibrariesMapType m_mapLibraries;

};


#endif /*PLUGINLOADSTRATEGY_H_*/
