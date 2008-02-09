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

	int GetRecords( hostRec& Host, int iRecordsCount );

	CTempStorage& operator<<( CTempStorage& );

	unsigned int GetSize();

	bool IsEof();

	void Clear();

	bool IsExists();

private:

	void Open( bool bRead );

	CTempStorage( const CTempStorage& );
	CTempStorage& operator=( const CTempStorage& );

	std::fstream m_sFile;
	std::string m_strFileName, m_strHostName, m_strHostAddr;

	std::istream::pos_type m_ReadPos;

	bool m_bOpenedForWrite;
};

#endif
