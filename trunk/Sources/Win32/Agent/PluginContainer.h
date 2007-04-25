//------------------------------------------------------------------------------------//
//Этот файл является частью проекта Exspecto 2006г.
//Module: Plugin load strategy
//Author: Parshin Dmitry
//Description: Класс, описывающий стратегию загрузки плагинов в класс Container
//-------------------------------------------------------------------------------------//
#ifndef PLUGINLOADSTRATEGY_H_
#define PLUGINLOADSTRATEGY_H_

#include <string>
#include <vector>
#include "PluginInterface.h"

class PluginContainer
{
public:
	//Классы исключений, генерируемые PluginLoadStrategy
	class PluginLoadErr:public std::runtime_error
	{
	public:
		PluginLoadErr( const std::string& strErr )throw():std::runtime_error( strErr ){};
		virtual ~PluginLoadErr()throw(){};
	};
	
	PluginContainer();
	virtual ~PluginContainer();
	
	//Тип итератора для манипуляций с содержимым контейнера
	typedef std::map< std::string, ScanFunc >::iterator iterator;

	//Возвращает итератор первого элемента контейнера	
	iterator begin()
	{
		return m_mapScanners.begin();
	}
	
	//Возвращает итератор следующего за последним элемента контейнера	
	iterator end()
	{
		return m_mapScanners.end();
	}
	
	//Возвращает кол-во элементов контейнера
	int count()
	{	
		return m_mapScanners.size();
	}	
private:	
	PluginContainer( const PluginContainer& );
	PluginContainer& operator=( const PluginContainer& );

	std::vector< HMODULE > m_vecLibraries;

	std::map< std::string, ScanFunc > m_mapScanners;

};


#endif /*PLUGINLOADSTRATEGY_H_*/
