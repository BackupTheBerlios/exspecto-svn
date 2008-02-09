//------------------------------------------------------------------------------------//
//Module: Plugin load strategy
//Author: Parshin Dmitry
//Description:
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

  class PluginLoadErr:public std::runtime_error
	{
	public:
	PluginLoadErr( const std::string& strErr )throw():std::runtime_error( strErr ){};
	  virtual ~PluginLoadErr()throw(){};
	};

  PluginContainer();
  virtual ~PluginContainer();

  typedef std::map< std::string, ScanFunc >::iterator iterator;

  iterator begin()
  {
	return m_mapScanners.begin();
  }

  iterator end()
  {
	return m_mapScanners.end();
  }

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
