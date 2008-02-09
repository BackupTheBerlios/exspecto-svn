//------------------------------------------------------------------------------------//
//���͜�����ޜ��� ������М��ٜ��� ������Ҝ��ۜ�����՜��������� ������М������������ ���ߜ������ޜ��՜��ڜ������ Exspecto 2006����.
//Module: Plugin load strategy
//Author: Parshin Dmitry
//Description: �������ۜ��М������, ���ޜ��ߜ��؜��������Ҝ��М��������؜��� �������������М�����՜��Ӝ��؜��� ���ל��М��Ӝ���������ל��ڜ��� ���ߜ��ۜ��М��Ӝ��؜��ݜ��ޜ��� ���� ���ڜ��ۜ��М������ Container
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
	//�������ۜ��М��������� ���؜�����ڜ��ۜ��������՜��ݜ��؜���, ���Ӝ��՜��ݜ��՜������؜���������՜��ܜ������ PluginLoadStrategy
	class PluginLoadErr:public std::runtime_error
	{
	public:
		PluginLoadErr( const std::string& strErr )throw():std::runtime_error( strErr ){};
		virtual ~PluginLoadErr()throw(){};
	};

	PluginContainer();
	virtual ~PluginContainer();

	//�����؜��� ���؜�����՜������М�����ޜ������� ���Ԝ��ۜ��� ���ܜ��М��ݜ��؜��ߜ�����ۜ��������؜��� ���� ������ޜ��Ԝ��՜������֜��؜��ܜ������ ���ڜ��ޜ��ݜ�����՜��ٜ��ݜ��՜�������
	typedef std::map< std::string, ScanFunc >::iterator iterator;

	//�������ޜ��ל��Ҝ������М�����М��՜��� ���؜�����՜������М�����ޜ��� ���ߜ��՜������Ҝ��ޜ��Ӝ��� ������ۜ��՜��ܜ��՜��ݜ������ ���ڜ��ޜ��ݜ�����՜��ٜ��ݜ��՜�������
	iterator begin()
	{
		return m_mapScanners.begin();
	}

	//�������ޜ��ל��Ҝ������М�����М��՜��� ���؜�����՜������М�����ޜ��� ������ۜ��՜��Ԝ�����������՜��Ӝ��� ���ל��� ���ߜ��ޜ�����ۜ��՜��Ԝ��ݜ��؜��� ������ۜ��՜��ܜ��՜��ݜ������ ���ڜ��ޜ��ݜ�����՜��ٜ��ݜ��՜�������
	iterator end()
	{
		return m_mapScanners.end();
	}

	//�������ޜ��ל��Ҝ������М�����М��՜��� ���ڜ��ޜ���-���Ҝ��� ������ۜ��՜��ܜ��՜��ݜ�����ޜ��� ���ڜ��ޜ��ݜ�����՜��ٜ��ݜ��՜�������
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
