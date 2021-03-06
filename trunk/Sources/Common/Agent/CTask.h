#ifndef CTASK_H_
#define CTASK_H_

#include "ServerHandler.h"
#include "Event.h"
#include "MessageParser.h"

#include "PluginContainer.h"
#include "CScanner.h"
#include "ThreadsPool.h"
#include "CDBProvider.h"
#include "TempStorage.h"

class CTask
{
public:

	CTask( CServerHandler& Handler ):m_ServerHandler( Handler )
	{};

	virtual ~CTask()
	{}

	virtual bool Immidiate() = 0;

	virtual void Execute( CEvent& CancelEv ) = 0;

	virtual void Load( CInPacket& Msg ) = 0;
/*
	void Cancel()
	{
		Log::instance().Trace( 80, "%s: ", __FUNCTION__ );
		m_CancelEv.Set();
	};
*/
	virtual std::string GetDescription() = 0;

protected:

	CTask( const CTask& );
	CTask& operator=( const CTask& );

	CServerHandler m_ServerHandler;

	static std::string m_CurState;

	static CMutex m_mtxCurState;

	static std::map< std::string, std::map< std::string, SmartPtr<CTempStorage> > > m_mapStorages;

	typedef std::map< std::string, std::map< std::string, SmartPtr<CTempStorage> > >::iterator StoragesIt;

	static CMutex m_mtxStorages;

	//static CEvent m_CancelEv;

};

class CGetStatus: public CTask
{
public:

	CGetStatus( CServerHandler& Handler ):CTask( Handler )
	{};

	virtual bool Immidiate();

	virtual void Execute( CEvent& CancelEv ){};

	virtual void Load( CInPacket& Msg ){};

	virtual std::string GetDescription()
	{
		return "Get agent status";
	};

};



class CStopScan: public CTask
{
public:

	CStopScan( CServerHandler& Handler ):CTask( Handler ){};

	virtual bool Immidiate();

	virtual void Execute( CEvent& CancelEv ){};

	virtual void Load( CInPacket& Msg ){};

	virtual std::string GetDescription()
	{
		return "Stop scan process";
	};

};



class CGetData: public CTask
{
public:

	CGetData( CServerHandler& Handler ):CTask( Handler )
									   ,m_iPacketSize(0)
	{};

	virtual bool Immidiate();

	virtual void Execute( CEvent& CancelEv ){};

	virtual void Load( CInPacket& Msg );

	virtual std::string GetDescription()
	{
		return "Get scan result";
	};
private:

	int m_iPacketSize;
};

class CStartScan: public CTask
{
public:
	class CScanThreadTask: public CThreadTask
	{
	public:

		CScanThreadTask( const std::string& strAddr, ScanFunc pScanner );
		virtual ~CScanThreadTask(){};

		virtual void Execute( CEvent& CancelEvent );

	private:

		static void StorageFunc( const char* strAddress, const char* strProtocolName, const char* strFileName, long long FileSize, int lFileTime, int hFileTime );

		std::string m_strAddr;

		ScanFunc m_pScanner;

		CMutex m_mtxStore;
	};

	class CAvailabilityScanTask: public CThreadTask
	{
	public:
		CAvailabilityScanTask( const std::string& strAddress ):m_strAddr( strAddress )
		{};

		virtual ~CAvailabilityScanTask(){};

		virtual void Execute( CEvent& CancelEvent );

		bool IsAvailable(){ return m_bResult; }

		std::string GetAddress(){ return m_strAddr; }

	private:

		std::string m_strAddr;

		bool m_bResult;
	};

	class CResolveTask: public CThreadTask
	{
	public:
		CResolveTask( const std::string& strAddress ):m_strAddr( strAddress )
		{};

		virtual ~CResolveTask(){};

		virtual void Execute( CEvent& CancelEvent );

		std::string GetHostName(){ return m_strHostName; }

		std::string GetAddress(){ return m_strAddr; }

	private:

		std::string m_strAddr;

		std::string m_strHostName;
	};


	CStartScan( CServerHandler& Handler ):CTask( Handler )
	{
		m_strDescription = "�����������������������������ʜ����������������������������͜�������������Ȝ�������������М�������������Μ�����������������������������������������͜�������������Ȝ�������������� �����������������������������Ĝ�������������М�������������Ŝ�������������ќ�������������Μ��������������: ";
	};

	virtual bool Immidiate();

	virtual void Execute( CEvent& CancelEv );

	virtual void Load( CInPacket& Msg );

	virtual std::string GetDescription()
	{
		return m_strDescription;
	};

private:

	std::string m_strDescription;

	std::vector< std::string > m_vecAddresses;

	static PluginContainer m_PluginContainer;

	typedef PluginContainer::iterator PluginIterator;

};


#endif /*CTASK_H_*/
