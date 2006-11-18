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
	ParamTypeErr( const std::string& strParamName ):std::logic_error( "shqParams: Запрошен параметр неверного типа: " + strParamName ){};
};

class ParamNotFound: public std::runtime_error
{
public:
	ParamNotFound():std::runtime_error( "Необходимый параметр не найден" ){};
};

class ParamLoadErr: public std::runtime_error
{
public:
	ParamLoadErr( const std::string& strMsg ):std::runtime_error( strMsg ){};
};


/*!
	Базовый абстрактный класс параметров для хранения параметров 
	в ассоциативном массиве в  классе Settings. 
    Виртуальный деструктор необходим для корректного очищения памяти.
*/
class BaseParam
{
	public:
			/// Виртуальный деструктор
		virtual ~BaseParam(){};

}; // class BaseParam

/*!
	Шаблонный класс для хранения произвольных параметров.
	Является потомком BaseParam, следовательно может храниться 
	в ассоциативном массиве в классе Settings.
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
	Класс получения и задания типизированных параметров.
	Является синглтоном
*/
//-------------------------------------------------------------------------
class Settings 
{	
public:
	
	//Метод доступа к экземпляру класса Settings
	static Settings& instance()
	{
		//Если доступ осуществляется в первый раз - создать экземпляр
		if( NULL == Settings::m_pInstance )
		//TODO: разобраться с удалением объекта, может воспользоваться atexit()
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
		Шаблонная функция помещения параметров в ассоциативный массив параметров.
		(Шаблонной сделана для того, чтобы передавать константную ссылку)
		
			Param_key	- имя параметра
			Param_value	- значение параметра

		Выделяется память под новый параметр. 
		Параметр помещается в массив.
	*/

	template< class T > void PutParam ( std::string Param_key, const T& Param_value )
		{				
			IterParamType it_Param;

				// Выделяем память под новый параметр
			UniParam<T>* lpParam = new UniParam<T>(Param_value);

				// Находим итератор, указывающий на элемент в массиве с ключом Param_key
			it_Param = mParams.find( Param_key );
			
				// Если такой параметр уже существует в массиве
			if ( it_Param != mParams.end() )
			{
				delete it_Param->second;
					//То меняем его значение на новое
				it_Param->second = lpParam;						
			}
			else 
			{
				std::pair< ParamType::iterator, bool > pairInsert;
					// Добавляем  параметр в массив
				pairInsert = mParams.insert( ParamType::value_type( Param_key, lpParam ));
				
				assert( pairInsert.second );
			}			
		}
	
		/*!
			Шаблонная функция извлечения параметров из ассоциативного массива параметров.
			
				Param_key -  имя параметра
				Param_value-  ссылка на объект, в который должно быть помещено значение параметра
		

			Ищется в массиве параметр с заданным именем. 
			Если он найден, берется его значение и проверяется, 
			соответствует ли выбранный параметр типу, переданному в шаблоне. 
		*/

		template< class T > void GetParam ( std::string Param_key,   T& Param_value ) 
		{			
			ConstIterParamType it_Param;
				// Находим итератор, указывающий на элемент в массиве с ключом Param_key
			it_Param = mParams.find( Param_key );
				// Если такой элемент найден
			if ( it_Param != mParams.end() )
			{
					// Проверяем того ли он типа
				UniParam<T>* lpParam = dynamic_cast< UniParam<T>* >( it_Param->second );
					// Если да, 
				if( lpParam )
							// то возвращаем параметр
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
			Функция удаления параметра из ассоциативного массива параметров.
			
				Param_key -  имя параметра				
		
			Ищется в массиве параметр с заданным именем. 
			Если он найден, удаляем его.
		*/
		void DeleteParam ( std::string Param_key ) 
		{	
			IterParamType it_Param;
				// Находим итератор, указывающий на элемент в массиве с ключом Param_key
			it_Param = mParams.find( Param_key );
				// Если такой элемент найден                                                                                                                                                                                                           
			if ( it_Param != mParams.end() )
				mParams.erase( it_Param );			
			else
				throw ParamNotFound(); 	
		} // short DeleteParam

		/*!
			Функция очищения параметров.			
		*/
	  void ClearParams()
      {		   
		for( ParamType::iterator p = mParams.begin(); p != mParams.end(); p++ )
			delete p->second;
		mParams.clear();                  
      }
	
		/*!
			Деструктор.
			Очищает память, выделенную под каждый параметр.
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

	
	//Имя модуля
	static std::string m_strModuleName;
	
	/*!
		Ассоциативный массив параметров, в котором
			ключ - имя параметра
			значение - сам параметр ( хранится в виде указателя на базовый класс параметра )

		В качестве значения также может выступать указатель на  потомок базового класса. 
		{Для обеспечения полиморфизма хранится указатель, а не сам объект}
		Для хранения параметров используется класс UniParam, что позволяет хранить 
		в массиве объекты любых типов.			
	*/
	std::map < std::string, BaseParam * > mParams;
	/*!
		Тип массива параметров
	*/
	typedef std::map < std::string, BaseParam * > ParamType; 
	/*!
		Тип константного итератора массива
	*/
	typedef ParamType::const_iterator ConstIterParamType; 	
	/*!
		Тип итератора массива
	*/
	typedef ParamType::iterator IterParamType; 				
}; // class Settings

#endif 
