#ifndef _TEMP_STORAGE
#define _TEMP_STORAGE
#include <fstream>
#include "SmartPtr.hpp"

class TempStorageErr: public std::runtime_error
{
public:

	TempStorageErr( const std::string& strMsg ):std::runtime_error( strMsg ){};
	virtual ~TempStorageErr(){};
};


class CTempStorage
{
public:
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
		m_sFile.write( strData.c_str(), (std::streamsize)strData.size()+1 );
		m_sFile.close();
	}
/*
	template< class T > bool Get( T& data )
	{
		Open( true );
		m_sFile.get( (char*)&data, sizeof( T ) );
		m_sFile.close();
	}

	void Get( std::string& strData )
	{
		int iSize;
		Get( iSize );
		Open( true );
		strData.resize( iSize );
		m_sFile.get( (char*)&strData[0], iSize );
		m_sFile.close();
	}
*/
	long Size(); 

	SmartPtr<BYTE> GetBuf( unsigned long& ulCount );

	CTempStorage& operator<<( CTempStorage& );

	void Clear();

	bool IsExists();

private:

	void Open( bool bRead );

	CTempStorage( const CTempStorage& );
	CTempStorage& operator=( const CTempStorage& );

	std::fstream m_sFile;
	std::string m_strFileName;
};

#endif
