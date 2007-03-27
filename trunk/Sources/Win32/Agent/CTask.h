#ifndef CTASK_H_
#define CTASK_H_

#include "ServerHandler.h"
#include "Event.hpp"
#include "MessageParser.h"

#include "Container.hpp"
#include "PluginLoadStrategy.h"
#include "CScanner.h"

class CTask
{
public:
	
	CTask( CServerHandler& Handler ):m_ServerHandler( Handler )
	{};
	
	virtual ~CTask()
	{}
	
	virtual bool Immidiate() = 0;
	
	virtual void Execute() = 0;
	
	virtual void Load( CPacket& Msg ) = 0;
	
	void Cancel()
	{
		Log::instance().Trace( 80, "CTask::Cancel: ������ ��������" );
		m_CancelEv.Set();
	};
	
	virtual std::string GetDescription() = 0;
	
protected:

	CServerHandler m_ServerHandler;
	
	static enumAgentState m_CurState;
	
	static CCriticalSection m_csCurState; 	
	
	static std::vector< std::string > m_vecData;
	
	static CEvent m_CancelEv;
	
};

class CGetStatus: public CTask
{
public:
	
	CGetStatus( CServerHandler& Handler ):CTask( Handler )
	{};
	
	virtual bool Immidiate();
	
	virtual void Execute(){};
	
	virtual void Load( CPacket& Msg ){};
	
	virtual std::string GetDescription()
	{
		return "��������� ������� ������"; 
	};

};



class CStopScan: public CTask
{
public:
	
	CStopScan( CServerHandler& Handler ):CTask( Handler ){};
	
	virtual bool Immidiate();
	
	virtual void Execute(){};
	
	virtual void Load( CPacket& Msg ){};

	virtual std::string GetDescription()
	{
		return "������� ������������"; 
	};
	
};



class CGetData: public CTask
{
public:
	
	CGetData( CServerHandler& Handler ):CTask( Handler ){};
	
	virtual bool Immidiate();
	
	virtual void Execute(){};
	
	virtual void Load( CPacket& Msg ){};

	virtual std::string GetDescription()
	{
		return "��������� ������"; 
	};
	
};


class CStartScan: public CTask
{
public:
	
	CStartScan( CServerHandler& Handler ):CTask( Handler )
	{
		m_strDescription = "������������ �������:";
		for( std::vector< std::string >::const_iterator It = m_vecAddresses.begin(); It != m_vecAddresses.end(); It++ )
		{
			m_strDescription += *It;
			m_strDescription += " ";
		}
	};
	
	virtual bool Immidiate();
	
	virtual void Execute();
	
	virtual void Load( CPacket& Msg );
	
	virtual std::string GetDescription()
	{
		return m_strDescription; 
	};
	
private:

	std::string m_strDescription;

	std::vector< std::string > m_vecAddresses;
	
	//��������� ��������
	static Container< CScanner*, PluginLoadStrategy > m_PluginContainer;
	
	//��� �������� ��� ����������� � ����������� ��������
	typedef Container< CScanner*, PluginLoadStrategy >::iterator PluginIterator;
	
};


#endif /*CTASK_H_*/