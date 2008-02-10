#ifndef SETTINGSCONTAINER_H_
#define SETTINGSCONTAINER_H_

#include <map>
#include <list>
#include <string>
#include <stdexcept>
#include "constants.h"
#include "CLog.h"
#include <assert.h>
#include "Singleton.hpp"


class ParamErr: public std::runtime_error
{
public:
	ParamErr( const std::string& Msg ):std::runtime_error( Msg ){};

	virtual ~ParamErr()throw(){};
};

class ParamTypeErr: public ParamErr
{
public:
 ParamTypeErr( const std::string& strParamName ):ParamErr( std::string(__FUNCTION__) + ": parameter type error: " + strParamName ){};
};

class ParamNotFound: public ParamErr
{
public:
 ParamNotFound( const std::string strParamName ):ParamErr( std::string(__FUNCTION__) + ": parameter " + strParamName + " is not found" ){};
};

class ParamLoadErr: public ParamErr
{
public:
	ParamLoadErr( const std::string& strMsg ):ParamErr( strMsg ){};
};

class ParamSerializeErr: public ParamErr
{
public:
 ParamSerializeErr( const std::string& strParamName ):ParamErr( std::string(__FUNCTION__) + ": parameter parse error: " + strParamName ){};
};


class BaseParam
{
	public:

		virtual ~BaseParam(){};

}; // class BaseParam



template<typename _T>
class UniParam : public BaseParam
{
public:
	UniParam( _T src )
	{
		m_Param = src;
	}	

	_T m_Param;
};

//-------------------------------------------------------------------------
class CSettings 
{	
public:

	template< class T > void PutParam ( std::string Param_key, const T& Param_value )
		{				
			IterParamType it_Param;

			UniParam<T>* lpParam = new UniParam<T>(Param_value);

			it_Param = mParams.find( Param_key );
			
			if ( it_Param != mParams.end() )
			{
				delete it_Param->second;
				it_Param->second = lpParam;						
			}
			else 
			{
				std::pair< ParamType::iterator, bool > pairInsert;
				pairInsert = mParams.insert( ParamType::value_type( Param_key, lpParam ));
				assert( pairInsert.second );
			}			
		}
	
		template< class T > void GetParam ( std::string Param_key,   T& Param_value ) 
		{			
			ConstIterParamType it_Param;

			it_Param = mParams.find( Param_key );

			if ( it_Param != mParams.end() )
			{
				UniParam<T>* lpParam = dynamic_cast< UniParam<T>* >( it_Param->second );
				if( lpParam )
				{
					T ParamValue = lpParam->m_Param;
					Param_value = ParamValue;
				}
                else				
					throw ParamTypeErr( Param_key );    									
			}
			else
				throw ParamNotFound( Param_key ); 
		} // short GetParam

		void DeleteParam ( std::string Param_key ) 
		{	
			IterParamType it_Param;
			it_Param = mParams.find( Param_key );
			if ( it_Param != mParams.end() )
				mParams.erase( it_Param );			
			else
				throw ParamNotFound( Param_key ); 	
		} // short DeleteParam

	  void ClearParams()
      {		   
		for( ParamType::iterator p = mParams.begin(); p != mParams.end(); p++ )
			delete p->second;
		mParams.clear();                  
      }
	
	  ~CSettings()
      {		 
		  ClearParams();
      }

protected:

	CSettings();

private:		

	CSettings( const CSettings& s );
	CSettings& operator=( const CSettings& s );

	std::map < std::string, BaseParam * > mParams;

	typedef std::map < std::string, BaseParam * > ParamType; 
	typedef ParamType::const_iterator ConstIterParamType; 	
	typedef ParamType::iterator IterParamType; 				
}; // class CSettings

//-----------------------------------------------------------------------------------------------------------------
//---------------------------------------------CIniSettings----------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------
class CIniTypeSerializer;

class CIniSettings: public CSettings
{
	friend class CSingleton< CIniSettings >;
public:

	virtual ~CIniSettings();

	void SetModule( const std::string& strModuleName, char** pModuleParams, int iParamCount );

	static bool RegisterTypeSerializer( const std::string& strType, CIniTypeSerializer* TypeSer );

private:

	CIniSettings();
	CIniSettings( const CIniSettings& c );
	CIniSettings& operator=( const CIniSettings& c );

	static std::map< std::string, CIniTypeSerializer* > m_mapSerializers;
};

//-----------------------------------------------------------------------------------------------------------------
//---------------------------------------------CIniTypeSerializer----------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------
class CIniTypeSerializer
{
public:
	
	virtual ~CIniTypeSerializer(){};

	virtual void Load( const std::string& strParamName, const std::string& strParamValue, CSettings* pParamContainer ) = 0;
};

class CIniIntSerializer: public CIniTypeSerializer
{
public:

	virtual void Load( const std::string& strParamName, const std::string& strParamValue, CSettings* pParamContainer );
};

class CIniStringSerializer: public CIniTypeSerializer
{
public:

	virtual void Load( const std::string& strParamName, const std::string& strParamValue, CSettings* pParamContainer );
};

class CIniStringListSerializer: public CIniTypeSerializer
{
public:

	virtual void Load( const std::string& strParamName, const std::string& strParamValue, CSettings* pParamContainer );
};

class CIniBoolSerializer: public CIniTypeSerializer
{
public:

	virtual void Load( const std::string& strParamName, const std::string& strParamValue, CSettings* pParamContainer );
};

class CIniIpListSerializer: public CIniTypeSerializer
{
public:

	virtual void Load( const std::string& strParamName, const std::string& strParamValue, CSettings* pParamContainer );
};

//-----------------------------------------------------------------------------------------------------------------
//---------------------------------------------Tools---------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------

namespace Tools
{
	bool GetStringList( std::string strSource, std::list< std::string >& listDest );

	struct structIp
	{
		int A,B,C,D;
	};
	
	bool ParseIp( const std::string& strSource, structIp& resIp );
}
typedef CSingleton< CIniSettings > Settings;

#endif 
