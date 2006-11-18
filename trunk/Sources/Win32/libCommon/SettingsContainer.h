#ifndef SETTINGSCONTAINER_H_
#define SETTINGSCONTAINER_H_

#include <map>
#include <string>
#include <stdexcept>
#include "constants.h"
#include "CLog.h"


class ParamTypeErr: public std::logic_error
{
public:
	ParamTypeErr( const std::string& strParamName ):std::logic_error( "shqParams: �������� �������� ��������� ����: " + strParamName ){};
};

class ParamNotFound: public std::runtime_error
{
public:
	ParamNotFound():std::runtime_error( "����������� �������� �� ������" ){};
};

class ParamLoadErr: public std::runtime_error
{
public:
	ParamLoadErr( const std::string& strMsg ):std::runtime_error( strMsg ){};
};


/*!
	������� ����������� ����� ���������� ��� �������� ���������� 
	� ������������� ������� �  ������ Settings. 
    ����������� ���������� ��������� ��� ����������� �������� ������.
*/
class BaseParam
{
	public:
			/// ����������� ����������
		virtual ~BaseParam(){};

}; // class BaseParam

/*!
	��������� ����� ��� �������� ������������ ����������.
	�������� �������� BaseParam, ������������� ����� ��������� 
	� ������������� ������� � ������ Settings.
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
	����� ��������� � ������� �������������� ����������.
	�������� ����������
*/
//-------------------------------------------------------------------------
class Settings 
{	
public:
	
	//����� ������� � ���������� ������ Settings
	static Settings& instance()
	{
		//���� ������ �������������� � ������ ��� - ������� ���������
		if( NULL == Settings::m_pInstance )
		//TODO: ����������� � ��������� �������, ����� ��������������� atexit()
			Settings::m_pInstance = new Settings();
		return *Settings::m_pInstance;
	}
	
	static void SetModule( const char* strModuleName, char** pModuleParams, int iParamCount )
	{
		m_strModuleName = strModuleName;
		m_pParamTypes = pModuleParams;
		m_iParamCount = iParamCount;
	}
	
	/*!
		��������� ������� ��������� ���������� � ������������� ������ ����������.
		(��������� ������� ��� ����, ����� ���������� ����������� ������)
		
			Param_key	- ��� ���������
			Param_value	- �������� ���������

		���������� ������ ��� ����� ��������. 
		�������� ���������� � ������.
	*/

	template< class T > void PutParam ( std::string Param_key, const T& Param_value )
		{				
			IterParamType it_Param;

				// �������� ������ ��� ����� ��������
			UniParam<T>* lpParam = new UniParam<T>(Param_value);

				// ������� ��������, ����������� �� ������� � ������� � ������ Param_key
			it_Param = mParams.find( Param_key );
			
				// ���� ����� �������� ��� ���������� � �������
			if ( it_Param != mParams.end() )
			{
				delete it_Param->second;
					//�� ������ ��� �������� �� �����
				it_Param->second = lpParam;						
			}
			else 
			{
				std::pair< ParamType::iterator, bool > pairInsert;
					// ���������  �������� � ������
				pairInsert = mParams.insert( ParamType::value_type( Param_key, lpParam ));
				
				assert( pairInsert.second );
			}			
		}
	
		/*!
			��������� ������� ���������� ���������� �� �������������� ������� ����������.
			
				Param_key -  ��� ���������
				Param_value-  ������ �� ������, � ������� ������ ���� �������� �������� ���������
		

			������ � ������� �������� � �������� ������. 
			���� �� ������, ������� ��� �������� � �����������, 
			������������� �� ��������� �������� ����, ����������� � �������. 
		*/

		template< class T > void GetParam ( std::string Param_key,   T& Param_value ) 
		{			
			ConstIterParamType it_Param;
				// ������� ��������, ����������� �� ������� � ������� � ������ Param_key
			it_Param = mParams.find( Param_key );
				// ���� ����� ������� ������
			if ( it_Param != mParams.end() )
			{
					// ��������� ���� �� �� ����
				UniParam<T>* lpParam = dynamic_cast< UniParam<T>* >( it_Param->second );
					// ���� ��, 
				if( lpParam )
							// �� ���������� ��������
				{
					T ParamValue = lpParam->m_Param;
					Param_value = ParamValue;
				}
                else				
					throw ParamTypeErr( Param_key );    									
				
			}
			else
				throw ParamNotFound(); 
		} // short GetParam

		/*!
			������� �������� ��������� �� �������������� ������� ����������.
			
				Param_key -  ��� ���������				
		
			������ � ������� �������� � �������� ������. 
			���� �� ������, ������� ���.
		*/
		void DeleteParam ( std::string Param_key ) 
		{	
			IterParamType it_Param;
				// ������� ��������, ����������� �� ������� � ������� � ������ Param_key
			it_Param = mParams.find( Param_key );
				// ���� ����� ������� ������                                                                                                                                                                                                           
			if ( it_Param != mParams.end() )
				mParams.erase( it_Param );			
			else
				throw ParamNotFound(); 	
		} // short DeleteParam

		/*!
			������� �������� ����������.			
		*/
	  void ClearParams()
      {		   
		for( ParamType::iterator p = mParams.begin(); p != mParams.end(); p++ )
			delete p->second;
		mParams.clear();                  
      }
	
		/*!
			����������.
			������� ������, ���������� ��� ������ ��������.
		*/
		~Settings()
      {		 
		  ClearParams();
      }

private:		
	
	Settings();	

	static Settings* m_pInstance;
	
	static char** m_pParamTypes;
	
	static int m_iParamCount;

	
	//��� ������
	static std::string m_strModuleName;
	
	/*!
		������������� ������ ����������, � �������
			���� - ��� ���������
			�������� - ��� �������� ( �������� � ���� ��������� �� ������� ����� ��������� )

		� �������� �������� ����� ����� ��������� ��������� ��  ������� �������� ������. 
		{��� ����������� ������������ �������� ���������, � �� ��� ������}
		��� �������� ���������� ������������ ����� UniParam, ��� ��������� ������� 
		� ������� ������� ����� �����.			
	*/
	std::map < std::string, BaseParam * > mParams;
	/*!
		��� ������� ����������
	*/
	typedef std::map < std::string, BaseParam * > ParamType; 
	/*!
		��� ������������ ��������� �������
	*/
	typedef ParamType::const_iterator ConstIterParamType; 	
	/*!
		��� ��������� �������
	*/
	typedef ParamType::iterator IterParamType; 				
}; // class Settings

#endif 
