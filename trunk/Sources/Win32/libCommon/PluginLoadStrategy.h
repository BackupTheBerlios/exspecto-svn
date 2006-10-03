//------------------------------------------------------------------------------------//
//Этот файл является частью проекта Exspecto 2006г.
//Module: Plugin load strategy
//Author: Parshin Dmitry
//Description: Класс, описывающий стратегию загрузки плагинов в класс Container
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
	
	//Типы функций, содержащихся в dll с plugin-ом
	typedef CScanner* (*fnGetScannerFunc)();
	typedef void (*fnReleaseScannerFunc)();
	

	typedef std::map< HINSTANCE , fnReleaseScannerFunc > LibrariesMapType;
	//Вектор дескрипторов открытых библиотек и соответствующих им функций очистки 
	LibrariesMapType m_mapLibraries;

};


#endif /*PLUGINLOADSTRATEGY_H_*/
