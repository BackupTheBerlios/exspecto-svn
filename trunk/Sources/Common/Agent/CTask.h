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
		Log::instance().Trace( 80, "CTask::Cancel: œô¨¾œô¨âœô¨Üœô¨Õœô¨Ýœô¨Ð œô¨Þœô¨ßœô¨Õœô¨àœô¨Ðœô¨æœô¨Øœô¨Ø" );
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

	//œô¨°œô¨Ôœô¨àœô¨Õœô¨á,œô¨Øœô¨Üœô¨ï œô¨ßœô¨àœô¨Þœô¨Þœô¨âœô¨Þœô¨Úœô¨Þœô¨Ûœô¨Ð,œô¨åœô¨àœô¨Ðœô¨Ýœô¨Øœô¨Ûœô¨Øœô¨éœô¨Õ
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
		return "œô¨¿œô¨Þœô¨Ûœô¨ãœô¨çœô¨Õœô¨Ýœô¨Øœô¨Õ œô¨áœô¨âœô¨Ðœô¨âœô¨ãœô¨áœô¨Ð œô¨Ðœô¨Óœô¨Õœô¨Ýœô¨âœô¨Ð";
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
		return "œô¨¾œô¨áœô¨âœô¨Ðœô¨Ýœô¨Þœô¨Ò œô¨áœô¨Úœô¨Ðœô¨Ýœô¨Øœô¨àœô¨Þœô¨Òœô¨Ðœô¨Ýœô¨Øœô¨ï";
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
		return "œô¨¿œô¨Þœô¨Ûœô¨ãœô¨çœô¨Õœô¨Ýœô¨Øœô¨Õ œô¨Ôœô¨Ðœô¨Ýœô¨Ýœô¨ëœô¨å";
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
		m_strDescription = "œô¨Áœô¨Úœô¨Ðœô¨Ýœô¨Øœô¨àœô¨Þœô¨Òœô¨Ðœô¨Ýœô¨Øœô¨Õ œô¨Ðœô¨Ôœô¨àœô¨Õœô¨áœô¨Þœô¨Ò: ";
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

	//œô¨ºœô¨Þœô¨Ýœô¨âœô¨Õœô¨Ùœô¨Ýœô¨Õœô¨à œô¨ßœô¨Ûœô¨Ðœô¨Óœô¨Øœô¨Ýœô¨Þœô¨Ò
	static PluginContainer m_PluginContainer;

	//œô¨Âœô¨Øœô¨ß œô¨Øœô¨âœô¨Õœô¨àœô¨Ðœô¨âœô¨Þœô¨à œô¨Ôœô¨Ûœô¨ï œô¨Üœô¨Ðœô¨Ýœô¨Øœô¨ßœô¨ãœô¨Ûœô¨ïœô¨æœô¨Øœô¨Ù œô¨á œô¨Úœô¨Þœô¨Ýœô¨âœô¨Õœô¨Ùœô¨Ýœô¨Õœô¨àœô¨Þœô¨Ü œô¨ßœô¨Ûœô¨Ðœô¨Óœô¨Øœô¨Ýœô¨Þœô¨Ò
	typedef PluginContainer::iterator PluginIterator;

};


#endif /*CTASK_H_*/
