//------------------------------------------------------------------------------------//
//���� ���� �������� ������ ������� Exspecto 2006�.
//Module: Plugin load strategy
//Author: Parshin Dmitry
//Description: �����, ����������� ��������� �������� �������� � ����� Container
//-------------------------------------------------------------------------------------//
#ifndef PLUGINLOADSTRATEGY_H_
#define PLUGINLOADSTRATEGY_H_

#include "CScanner.h"

class PluginLoadStrategy
{
public:
	//������ ����������, ������������ PluginLoadStrategy
	class PluginLoadErr:public std::runtime_error
	{
	public:
		PluginLoadErr( const std::string& strErr )throw():std::runtime_error( strErr ){};
		virtual ~PluginLoadErr()throw(){};
	};
	
	PluginLoadStrategy( std::vector< CScanner* >& vecStorage );
	virtual ~PluginLoadStrategy();
	
private:	
	//���� �������, ������������ � dll � plugin-��
	typedef CScanner* (*fnGetScannerFunc)();
	typedef void (*fnReleaseScannerFunc)();
	

	typedef std::map< HINSTANCE , fnReleaseScannerFunc > LibrariesMapType;
	//������ ������������ �������� ��������� � ��������������� �� ������� ������� 
	LibrariesMapType m_mapLibraries;

};


#endif /*PLUGINLOADSTRATEGY_H_*/
