#ifndef CTASK_H_
#define CTASK_H_

#include "ServerHandler.h"
#include "Event.hpp"
#include "MessageParser.h"

#include "Container.hpp"
#include "PluginLoadStrategy.h"
#include "CScanner.h"
#include "ThreadsPool.h"

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
		Log::instance().Trace( 80, "CTask::Cancel: Отмена операции" );
		m_CancelEv.Set();
	};
	
	virtual std::string GetDescription() = 0;
	
protected:

	CTask( const CTask& );
	CTask& operator=( const CTask& );

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
		return "Получение статуса агента"; 
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
		return "Останов сканирования"; 
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
		return "Получение данных"; 
	};
	
};

class CStartScan: public CTask
{
public:
	class CScanThreadTask: public CThreadTask
	{
	public:

		CScanThreadTask( const std::string& strAddr, CScanner* pScanner );
		virtual ~CScanThreadTask(){};

		virtual void Execute( const CEvent& CancelEvent );

		void GetResData( std::vector< std::string >& vecResult );

	private:

		std::string m_strAddr;

		std::vector< std::string > m_vecData;

		CScanner* m_pScanner;
	};

	class CAvailabilityScanTask: public CThreadTask
	{
	public:
		CAvailabilityScanTask( const std::string& strAddress ):m_strAddr( strAddress )
		{};

		virtual ~CAvailabilityScanTask(){};

		virtual void Execute( const CEvent& CancelEvent );

		bool IsAvailable(){ return m_bResult; }

		std::string GetAddress(){ return m_strAddr; }

	private:

		std::string m_strAddr;

		bool m_bResult;
	};
	


	CStartScan( CServerHandler& Handler ):CTask( Handler )
	{
		m_strDescription = "Сканирование адресов:";
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
	
	//Контейнер плагинов
	static Container< CScanner*, PluginLoadStrategy > m_PluginContainer;
	
	//Тип итератор для манипуляций с контейнером плагинов
	typedef Container< CScanner*, PluginLoadStrategy >::iterator PluginIterator;
	
};


#endif /*CTASK_H_*/
