#ifndef _TEMP_STORAGE
#define _TEMP_STORAGE
#include <fstream>
#include "SmartPtr.hpp"
#include <stdexcept>

class CTempStorage
{
public:
	class TempStorageErr: public std::runtime_error
	{
	public:
	
		TempStorageErr( const std::string& strMsg )throw():std::runtime_error( strMsg ){};
		virtual ~TempStorageErr()throw(){};
	};
	CTempStorage( const std::string& strFileName );
	virtual ~CTempStorage(void);

	template< class T > void Put( T& data )
	{
		Open( false );
		m_sFile.write( (char*)&data, sizeof( T ) );
		m_sFile.close();
	}

	void Put( std::string strData )
	{
		Open( false );
		m_sFile.write( strData.c_str(), (std::streamsize)strData.size() + 1 );
		m_sFile.close();
	}

	void Put( const char* strData )
	{
		Open( false );
		m_sFile.write( strData, (std::streamsize)strlen( strData) + 1 );
		m_sFile.close();
	}


	long Size(); 

	SmartPtr<BYTE, AllocNewArray<BYTE> > GetBuf( unsigned long& ulCount );

	CTempStorage& operator<<( CTempStorage& );

	void Clear();

	bool IsExists();

private:

	unsigned long m_ulReadOffset;

	void Open( bool bRead );

	CTempStorage( const CTempStorage& );
	CTempStorage& operator=( const CTempStorage& );

	std::fstream m_sFile;
	std::string m_strFileName;
};

#endif
