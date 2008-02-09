//------------------------------------------------------------------------------------//
//œô¨Íœô¨âœô¨Þœô¨â œô¨äœô¨Ðœô¨Ùœô¨Û œô¨ïœô¨Òœô¨Ûœô¨ïœô¨Õœô¨âœô¨áœô¨ï œô¨çœô¨Ðœô¨áœô¨âœô¨ìœô¨î œô¨ßœô¨àœô¨Þœô¨Õœô¨Úœô¨âœô¨Ð Exspecto 2006œô¨Ó.
//Module: Plugin load strategy
//Author: Parshin Dmitry
//Description: œô¨ºœô¨Ûœô¨Ðœô¨áœô¨á, œô¨Þœô¨ßœô¨Øœô¨áœô¨ëœô¨Òœô¨Ðœô¨îœô¨éœô¨Øœô¨Ù œô¨áœô¨âœô¨àœô¨Ðœô¨âœô¨Õœô¨Óœô¨Øœô¨î œô¨×œô¨Ðœô¨Óœô¨àœô¨ãœô¨×œô¨Úœô¨Ø œô¨ßœô¨Ûœô¨Ðœô¨Óœô¨Øœô¨Ýœô¨Þœô¨Ò œô¨Ò œô¨Úœô¨Ûœô¨Ðœô¨áœô¨á Container
//-------------------------------------------------------------------------------------//
#ifndef PLUGINLOADSTRATEGY_H_
#define PLUGINLOADSTRATEGY_H_

#include <string>
#include <vector>
#include "PluginInterface.h"
#include "SharedLib.h"
#include "SmartPtr.hpp"

class PluginContainer
{
public:
	//œô¨ºœô¨Ûœô¨Ðœô¨áœô¨áœô¨ë œô¨Øœô¨áœô¨Úœô¨Ûœô¨îœô¨çœô¨Õœô¨Ýœô¨Øœô¨Ù, œô¨Óœô¨Õœô¨Ýœô¨Õœô¨àœô¨Øœô¨àœô¨ãœô¨Õœô¨Üœô¨ëœô¨Õ PluginLoadStrategy
	class PluginLoadErr:public std::runtime_error
	{
	public:
		PluginLoadErr( const std::string& strErr )throw():std::runtime_error( strErr ){};
		virtual ~PluginLoadErr()throw(){};
	};

	PluginContainer();
	virtual ~PluginContainer();

	//œô¨Âœô¨Øœô¨ß œô¨Øœô¨âœô¨Õœô¨àœô¨Ðœô¨âœô¨Þœô¨àœô¨Ð œô¨Ôœô¨Ûœô¨ï œô¨Üœô¨Ðœô¨Ýœô¨Øœô¨ßœô¨ãœô¨Ûœô¨ïœô¨æœô¨Øœô¨Ù œô¨á œô¨áœô¨Þœô¨Ôœô¨Õœô¨àœô¨Öœô¨Øœô¨Üœô¨ëœô¨Ü œô¨Úœô¨Þœô¨Ýœô¨âœô¨Õœô¨Ùœô¨Ýœô¨Õœô¨àœô¨Ð
	typedef std::map< std::string, ScanFunc >::iterator iterator;

	//œô¨²œô¨Þœô¨×œô¨Òœô¨àœô¨Ðœô¨éœô¨Ðœô¨Õœô¨â œô¨Øœô¨âœô¨Õœô¨àœô¨Ðœô¨âœô¨Þœô¨à œô¨ßœô¨Õœô¨àœô¨Òœô¨Þœô¨Óœô¨Þ œô¨íœô¨Ûœô¨Õœô¨Üœô¨Õœô¨Ýœô¨âœô¨Ð œô¨Úœô¨Þœô¨Ýœô¨âœô¨Õœô¨Ùœô¨Ýœô¨Õœô¨àœô¨Ð
	iterator begin()
	{
		return m_mapScanners.begin();
	}

	//œô¨²œô¨Þœô¨×œô¨Òœô¨àœô¨Ðœô¨éœô¨Ðœô¨Õœô¨â œô¨Øœô¨âœô¨Õœô¨àœô¨Ðœô¨âœô¨Þœô¨à œô¨áœô¨Ûœô¨Õœô¨Ôœô¨ãœô¨îœô¨éœô¨Õœô¨Óœô¨Þ œô¨×œô¨Ð œô¨ßœô¨Þœô¨áœô¨Ûœô¨Õœô¨Ôœô¨Ýœô¨Øœô¨Ü œô¨íœô¨Ûœô¨Õœô¨Üœô¨Õœô¨Ýœô¨âœô¨Ð œô¨Úœô¨Þœô¨Ýœô¨âœô¨Õœô¨Ùœô¨Ýœô¨Õœô¨àœô¨Ð
	iterator end()
	{
		return m_mapScanners.end();
	}

	//œô¨²œô¨Þœô¨×œô¨Òœô¨àœô¨Ðœô¨éœô¨Ðœô¨Õœô¨â œô¨Úœô¨Þœô¨Û-œô¨Òœô¨Þ œô¨íœô¨Ûœô¨Õœô¨Üœô¨Õœô¨Ýœô¨âœô¨Þœô¨Ò œô¨Úœô¨Þœô¨Ýœô¨âœô¨Õœô¨Ùœô¨Ýœô¨Õœô¨àœô¨Ð
	int count()
	{
		return m_mapScanners.size();
	}
private:
	PluginContainer( const PluginContainer& );
	PluginContainer& operator=( const PluginContainer& );

	std::vector< SmartPtr<CSharedLib> > m_vecLibraries;

	std::map< std::string, ScanFunc > m_mapScanners;

};


#endif /*PLUGINLOADSTRATEGY_H_*/
