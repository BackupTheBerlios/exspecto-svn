//------------------------------------------------------------------------------------//
//Этот файл является частью проекта Exspecto 2006г.
//Module: Plugin load strategy
//Author: Parshin Dmitry
//Description: Класс, описывающий стратегию загрузки плагинов в класс Container
//-------------------------------------------------------------------------------------//
#ifndef PLUGINLOADSTRATEGY_H_
#define PLUGINLOADSTRATEGY_H_

#include "CScanner.h"
#include <string>
#include <vector>

class PluginLoadStrategy
{
public:
	//Классы исключений, генерируемые PluginLoadStrategy
	class PluginLoadErr:public std::runtime_error
	{
	public:
		PluginLoadErr( const std::string& strErr )throw():std::runtime_error( strErr ){};
		virtual ~PluginLoadErr()throw(){};
	};
	
	PluginLoadStrategy( std::vector< CScanner* >& vecStorage );
	virtual ~PluginLoadStrategy();
	
private:	
	PluginLoadStrategy( const PluginLoadStrategy& );
	PluginLoadStrategy& operator=( const PluginLoadStrategy& );

	//Типы функций, содержащихся в dll с plugin-ом
	typedef CScanner* (*fnGetScannerFunc)();
	typedef void (*fnReleaseScannerFunc)();
	

	typedef std::map< HINSTANCE , fnReleaseScannerFunc > LibrariesMapType;
	//Вектор дескрипторов открытых библиотек и соответствующих им функций очистки 
	LibrariesMapType m_mapLibraries;

};


#endif /*PLUGINLOADSTRATEGY_H_*/
