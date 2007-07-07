#ifndef _TEMP_STORAGE
#define _TEMP_STORAGE
#include <fstream>
#include "SmartPtr.hpp"
#include <stdexcept>
#include "CDBProvider.h"
#include <vector>

class CTempStorage
{
public:

	class TempStorageErr: public std::runtime_error
	{
	public:
	
		TempStorageErr( const std::string& strMsg )throw():std::runtime_error( strMsg ){};
		virtual ~TempStorageErr()throw(){};
	};

	CTempStorage( const std::string& strHostName, const std::string& strHostAddr, const std::string& strProtoName );

	virtual ~CTempStorage(void);

	void PutRecord( const fileStr& File );

	std::string GetPacket( int iRecordsCount );

	void GetRecords( hostRec& Host, int iRecordsCount );

	CTempStorage& operator<<( CTempStorage& );

	void Clear();

	bool IsExists();

private:

	void Open( bool bRead );

	CTempStorage( const CTempStorage& );
	CTempStorage& operator=( const CTempStorage& );

	std::fstream m_sFile;
	std::string m_strFileName, m_strHostName, m_strHostAddr;

	//сути - флаг создания файла
	bool m_bOpenedForWrite;
};

#endif
