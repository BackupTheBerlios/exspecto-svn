#ifndef _TEMP_STORAGE
#define _TEMP_STORAGE
#include <fstream>

class CTempStorage
{
public:
	CTempStorage( const std::string& strFileName );
	virtual ~CTempStorage(void);

	std::fstream& GetStream()
	{
		return m_file;
	}

	CTempStorage& operator<<( CTempStorage& );

private:

	CTempStorage( const CTempStorage& );
	CTempStorage& operator=( const CTempStorage& );

	std::fstream m_file;
};

#endif
