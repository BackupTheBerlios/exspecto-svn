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


//����������ڜ��ۜ��������՜��ݜ��؜���, ���Ӝ��՜��ݜ��՜������؜���������՜��ܜ������ CSocket ���� ���ڜ��ۜ��М��������М��ܜ���, ���ݜ��М�����ۜ��՜��Ԝ�����������؜��ܜ��� ���ޜ��� ���ݜ��՜��Ӝ���, ���� ���������ݜ��ڜ�����؜�����ܜ��� ���؜��� Tools
class ParamErr: public std::runtime_error
{
public:
	ParamErr( const std::string& Msg ):std::runtime_error( Msg ){};

	virtual ~ParamErr()throw(){};
};

class ParamTypeErr: public ParamErr
{
public:
	ParamTypeErr( const std::string& strParamName ):ParamErr( "shqParams: �������М��ߜ������ޜ�����՜��� ���ߜ��М������М��ܜ��՜������ ���ݜ��՜��Ҝ��՜������ݜ��ޜ��Ӝ��� ������؜��ߜ���: " + strParamName ){};
};

class ParamNotFound: public ParamErr
{
public:
	ParamNotFound( const std::string strParamName ):ParamErr( "�������՜��ޜ��ќ�����ޜ��Ԝ��؜��ܜ������ ���ߜ��М������М��ܜ��՜������ " + strParamName + " ���ݜ��� ���ݜ��М��ٜ��Ԝ��՜���" ){};
};

class ParamLoadErr: public ParamErr
{
public:
	ParamLoadErr( const std::string& strMsg ):ParamErr( strMsg ){};
};

class ParamSerializeErr: public ParamErr
{
public:
	ParamSerializeErr( const std::string& strParamName ):ParamErr( "����������؜��ќ��ڜ��� ���ל��М��Ӝ���������ל��ڜ��� ���ߜ��М������М��ܜ��՜���������� " + strParamName ){};
};

/*!
	�������М��ל��ޜ��Ҝ������ ���М��ќ������������М��ڜ�����ݜ������ ���ڜ��ۜ��М������ ���ߜ��М������М��ܜ��՜���������ޜ��� ���Ԝ��ۜ��� ����������М��ݜ��՜��ݜ��؜��� ���ߜ��М������М��ܜ��՜���������ޜ��� 
	���� ���М��������ޜ�����؜��М�����؜��Ҝ��ݜ��ޜ��� ���ܜ��М��������؜��Ҝ��� ����  ���ڜ��ۜ��М��������� CSettings. 
    �������؜������������М��ۜ�����ݜ������ ���Ԝ��՜���������������ڜ�����ޜ��� ���ݜ��՜��ޜ��ќ�����ޜ��Ԝ��؜��� ���Ԝ��ۜ��� ���ڜ��ޜ����������՜��ڜ�����ݜ��ޜ��Ӝ��� ���ޜ�����؜�����՜��ݜ��؜��� ���ߜ��М��ܜ���������.
*/
class BaseParam
{
	public:
			/// �������؜������������М��ۜ�����ݜ������ ���Ԝ��՜���������������ڜ�����ޜ���
		virtual ~BaseParam(){};

}; // class BaseParam

/*!
	���Ȝ��М��ќ��ۜ��ޜ��ݜ��ݜ������ ���ڜ��ۜ��М������ ���Ԝ��ۜ��� ����������М��ݜ��՜��ݜ��؜��� ���ߜ������ޜ��؜��ל��Ҝ��ޜ��ۜ�����ݜ������ ���ߜ��М������М��ܜ��՜���������ޜ���.
	���Ϝ��Ҝ��ۜ�����՜��������� ���ߜ��ޜ�����ޜ��ܜ��ڜ��ޜ��� BaseParam, ������ۜ��՜��Ԝ��ޜ��Ҝ��М�����՜��ۜ�����ݜ��� ���ܜ��ޜ��֜��՜��� ����������М��ݜ��؜������������ 
	���� ���М��������ޜ�����؜��М�����؜��Ҝ��ݜ��ޜ��� ���ܜ��М��������؜��Ҝ��� ���� ���ڜ��ۜ��М��������� CSettings.
*/


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

/*!
	�������ۜ��М������ ���ߜ��ޜ��ۜ��������՜��ݜ��؜��� ���� ���ל��М��Ԝ��М��ݜ��؜��� ������؜��ߜ��؜��ל��؜������ޜ��Ҝ��М��ݜ��ݜ������ ���ߜ��М������М��ܜ��՜���������ޜ���.
	���Ϝ��Ҝ��ۜ�����՜��������� ������؜��ݜ��Ӝ��ۜ�����ޜ��ݜ��ޜ���
*/
//-------------------------------------------------------------------------
class CSettings 
{	
public:

	/*!
		���Ȝ��М��ќ��ۜ��ޜ��ݜ��ݜ��М��� ���������ݜ��ڜ�����؜��� ���ߜ��ޜ��ܜ��՜�����՜��ݜ��؜��� ���ߜ��М������М��ܜ��՜���������ޜ��� ���� ���М��������ޜ�����؜��М�����؜��Ҝ��ݜ������ ���ܜ��М��������؜��� ���ߜ��М������М��ܜ��՜���������ޜ���.
		(���Ȝ��М��ќ��ۜ��ޜ��ݜ��ݜ��ޜ��� ������Ԝ��՜��ۜ��М��ݜ��� ���Ԝ��ۜ��� ������ޜ��Ӝ���, ���������ޜ��ќ��� ���ߜ��՜������՜��Ԝ��М��Ҝ��М������ ���ڜ��ޜ��ݜ��������М��ݜ�����ݜ������ ������������ۜ��ڜ���)
		
			Param_key	- ���؜��ܜ��� ���ߜ��М������М��ܜ��՜����������
			Param_value	- ���ל��ݜ��М�����՜��ݜ��؜��� ���ߜ��М������М��ܜ��՜����������

		����������Ԝ��՜��ۜ�����՜��������� ���ߜ��М��ܜ��������� ���ߜ��ޜ��� ���ݜ��ޜ��Ҝ������ ���ߜ��М������М��ܜ��՜������. 
		�������М������М��ܜ��՜������ ���ߜ��ޜ��ܜ��՜�����М��՜��������� ���� ���ܜ��М��������؜���.
	*/

	template< class T > void PutParam ( std::string Param_key, const T& Param_value )
		{				
			IterParamType it_Param;

				// ����������Ԝ��՜��ۜ�����՜��� ���ߜ��М��ܜ��������� ���ߜ��ޜ��� ���ݜ��ޜ��Ҝ������ ���ߜ��М������М��ܜ��՜������
			UniParam<T>* lpParam = new UniParam<T>(Param_value);

				// �������М�����ޜ��Ԝ��؜��� ���؜�����՜������М�����ޜ���, ������ڜ��М��ל�����Ҝ��М��������؜��� ���ݜ��� ������ۜ��՜��ܜ��՜��ݜ��� ���� ���ܜ��М��������؜��Ҝ��� ���� ���ڜ��ۜ��������ޜ��� Param_key
			it_Param = mParams.find( Param_key );
			
				// ����������ۜ��� ������М��ڜ��ޜ��� ���ߜ��М������М��ܜ��՜������ ������֜��� ������������՜��������Ҝ�����՜��� ���� ���ܜ��М��������؜��Ҝ���
			if ( it_Param != mParams.end() )
			{
				delete it_Param->second;
					//������ ���ܜ��՜��ݜ�����՜��� ���՜��Ӝ��� ���ל��ݜ��М�����՜��ݜ��؜��� ���ݜ��� ���ݜ��ޜ��Ҝ��ޜ���
				it_Param->second = lpParam;						
			}
			else 
			{
				std::pair< ParamType::iterator, bool > pairInsert;
					// �������ޜ��ќ��М��Ҝ��ۜ�����՜���  ���ߜ��М������М��ܜ��՜������ ���� ���ܜ��М��������؜���
				pairInsert = mParams.insert( ParamType::value_type( Param_key, lpParam ));
				
				assert( pairInsert.second );
			}			
		}
	
		/*!
			���Ȝ��М��ќ��ۜ��ޜ��ݜ��ݜ��М��� ���������ݜ��ڜ�����؜��� ���؜��ל��Ҝ��ۜ��՜�����՜��ݜ��؜��� ���ߜ��М������М��ܜ��՜���������ޜ��� ���؜��� ���М��������ޜ�����؜��М�����؜��Ҝ��ݜ��ޜ��Ӝ��� ���ܜ��М��������؜��Ҝ��� ���ߜ��М������М��ܜ��՜���������ޜ���.
			
				Param_key -  ���؜��ܜ��� ���ߜ��М������М��ܜ��՜����������
				Param_value-  ������������ۜ��ڜ��� ���ݜ��� ���ޜ��ќ�����՜��ڜ���, ���� ���ڜ��ޜ�����ޜ���������� ���Ԝ��ޜ��ۜ��֜��ݜ��� ���ќ��������� ���ߜ��ޜ��ܜ��՜�����՜��ݜ��� ���ל��ݜ��М�����՜��ݜ��؜��� ���ߜ��М������М��ܜ��՜����������
		

			����������՜��������� ���� ���ܜ��М��������؜��Ҝ��� ���ߜ��М������М��ܜ��՜������ ���� ���ל��М��Ԝ��М��ݜ��ݜ������ ���؜��ܜ��՜��ݜ��՜���. 
			����������ۜ��� ���ޜ��� ���ݜ��М��ٜ��Ԝ��՜���, ���ќ��՜������՜��������� ���՜��Ӝ��� ���ל��ݜ��М�����՜��ݜ��؜��� ���� ���ߜ������ޜ��Ҝ��՜���������՜���������, 
			������ޜ��ޜ�����Ҝ��՜�����������Ҝ�����՜��� ���ۜ��� ���Ҝ�����ќ������М��ݜ��ݜ������ ���ߜ��М������М��ܜ��՜������ ������؜��ߜ���, ���ߜ��՜������՜��Ԝ��М��ݜ��ݜ��ޜ��ܜ��� ���� ������М��ќ��ۜ��ޜ��ݜ���. 
		*/

		template< class T > void GetParam ( std::string Param_key,   T& Param_value ) 
		{			
			ConstIterParamType it_Param;
				// �������М�����ޜ��Ԝ��؜��� ���؜�����՜������М�����ޜ���, ������ڜ��М��ל�����Ҝ��М��������؜��� ���ݜ��� ������ۜ��՜��ܜ��՜��ݜ��� ���� ���ܜ��М��������؜��Ҝ��� ���� ���ڜ��ۜ��������ޜ��� Param_key
			it_Param = mParams.find( Param_key );
				// ����������ۜ��� ������М��ڜ��ޜ��� ������ۜ��՜��ܜ��՜��ݜ��� ���ݜ��М��ٜ��Ԝ��՜���
			if ( it_Param != mParams.end() )
			{
					// �����������ޜ��Ҝ��՜���������՜��� ������ޜ��Ӝ��� ���ۜ��� ���ޜ��� ������؜��ߜ���
				UniParam<T>* lpParam = dynamic_cast< UniParam<T>* >( it_Param->second );
					// ����������ۜ��� ���Ԝ���, 
				if( lpParam )
							// ������� ���Ҝ��ޜ��ל��Ҝ������М�����М��՜��� ���ߜ��М������М��ܜ��՜������
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

		/*!
			���Ĝ�����ݜ��ڜ�����؜��� ������Ԝ��М��ۜ��՜��ݜ��؜��� ���ߜ��М������М��ܜ��՜���������� ���؜��� ���М��������ޜ�����؜��М�����؜��Ҝ��ݜ��ޜ��Ӝ��� ���ܜ��М��������؜��Ҝ��� ���ߜ��М������М��ܜ��՜���������ޜ���.
			
				Param_key -  ���؜��ܜ��� ���ߜ��М������М��ܜ��՜����������				
		
			����������՜��������� ���� ���ܜ��М��������؜��Ҝ��� ���ߜ��М������М��ܜ��՜������ ���� ���ל��М��Ԝ��М��ݜ��ݜ������ ���؜��ܜ��՜��ݜ��՜���. 
			����������ۜ��� ���ޜ��� ���ݜ��М��ٜ��Ԝ��՜���, ������Ԝ��М��ۜ�����՜��� ���՜��Ӝ���.
		*/
		void DeleteParam ( std::string Param_key ) 
		{	
			IterParamType it_Param;
				// �������М�����ޜ��Ԝ��؜��� ���؜�����՜������М�����ޜ���, ������ڜ��М��ל�����Ҝ��М��������؜��� ���ݜ��� ������ۜ��՜��ܜ��՜��ݜ��� ���� ���ܜ��М��������؜��Ҝ��� ���� ���ڜ��ۜ��������ޜ��� Param_key
			it_Param = mParams.find( Param_key );
				// ����������ۜ��� ������М��ڜ��ޜ��� ������ۜ��՜��ܜ��՜��ݜ��� ���ݜ��М��ٜ��Ԝ��՜���                                                                                                                                                                                                           
			if ( it_Param != mParams.end() )
				mParams.erase( it_Param );			
			else
				throw ParamNotFound( Param_key ); 	
		} // short DeleteParam

		/*!
			���Ĝ�����ݜ��ڜ�����؜��� ���ޜ�����؜�����՜��ݜ��؜��� ���ߜ��М������М��ܜ��՜���������ޜ���.			
		*/
	  void ClearParams()
      {		   
		for( ParamType::iterator p = mParams.begin(); p != mParams.end(); p++ )
			delete p->second;
		mParams.clear();                  
      }
	
		/*!
			�������՜���������������ڜ�����ޜ���.
			����������؜�����М��՜��� ���ߜ��М��ܜ���������, ���Ҝ�����Ԝ��՜��ۜ��՜��ݜ��ݜ������ ���ߜ��ޜ��� ���ڜ��М��֜��Ԝ������ ���ߜ��М������М��ܜ��՜������.
		*/
		~CSettings()
      {		 
		  ClearParams();
      }

protected:

	CSettings();

private:		

	CSettings( const CSettings& s );
	CSettings& operator=( const CSettings& s );
	/*!
		�������������ޜ�����؜��М�����؜��Ҝ��ݜ������ ���ܜ��М��������؜��� ���ߜ��М������М��ܜ��՜���������ޜ���, ���� ���ڜ��ޜ�����ޜ������ޜ���
			���ڜ��ۜ������ - ���؜��ܜ��� ���ߜ��М������М��ܜ��՜����������
			���ל��ݜ��М�����՜��ݜ��؜��� - ������М��� ���ߜ��М������М��ܜ��՜������ ( ����������М��ݜ��؜��������� ���� ���Ҝ��؜��Ԝ��� ������ڜ��М��ל��М�����՜��ۜ��� ���ݜ��� ���ќ��М��ל��ޜ��Ҝ������ ���ڜ��ۜ��М������ ���ߜ��М������М��ܜ��՜���������� )

		���� ���ڜ��М�����՜��������Ҝ��� ���ל��ݜ��М�����՜��ݜ��؜��� ������М��ڜ��֜��� ���ܜ��ޜ��֜��՜��� ���Ҝ��������������ߜ��М������ ������ڜ��М��ל��М�����՜��ۜ��� ���ݜ���  ���ߜ��ޜ�����ޜ��ܜ��ޜ��� ���ќ��М��ל��ޜ��Ҝ��ޜ��Ӝ��� ���ڜ��ۜ��М���������. 
		{�������ۜ��� ���ޜ��ќ��՜�����ߜ��՜�����՜��ݜ��؜��� ���ߜ��ޜ��ۜ��؜��ܜ��ޜ���������؜��ל��ܜ��� ����������М��ݜ��؜��������� ������ڜ��М��ל��М�����՜��ۜ���, ���� ���ݜ��� ������М��� ���ޜ��ќ�����՜��ڜ���}
		�������ۜ��� ����������М��ݜ��՜��ݜ��؜��� ���ߜ��М������М��ܜ��՜���������ޜ��� ���؜�����ߜ��ޜ��ۜ�����ל�����՜��������� ���ڜ��ۜ��М������ UniParam, ���������� ���ߜ��ޜ��ל��Ҝ��ޜ��ۜ�����՜��� ����������М��ݜ��؜������ 
		���� ���ܜ��М��������؜��Ҝ��� ���ޜ��ќ�����՜��ڜ������ ���ۜ�����ќ������ ������؜��ߜ��ޜ���.			
	*/
	std::map < std::string, BaseParam * > mParams;
	/*!
		�����؜��� ���ܜ��М��������؜��Ҝ��� ���ߜ��М������М��ܜ��՜���������ޜ���
	*/
	typedef std::map < std::string, BaseParam * > ParamType; 
	/*!
		�����؜��� ���ڜ��ޜ��ݜ��������М��ݜ�����ݜ��ޜ��Ӝ��� ���؜�����՜������М�����ޜ������� ���ܜ��М��������؜��Ҝ���
	*/
	typedef ParamType::const_iterator ConstIterParamType; 	
	/*!
		�����؜��� ���؜�����՜������М�����ޜ������� ���ܜ��М��������؜��Ҝ���
	*/
	typedef ParamType::iterator IterParamType; 				
}; // class CSettings

//�������՜��М��ۜ��؜��ל��М�����؜��� ���ל��М��Ӝ���������ל��ڜ��� ���ߜ��М������М��ܜ��՜���������ޜ��� ���؜��� ������М��ٜ��ۜ��� Settings.ini
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

//�������ۜ��М���������, �������՜��М��ۜ��؜��ל�����������؜��� ������՜������؜��М��ۜ��؜��ל��М�����؜��� ������؜��ߜ��ޜ���
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
