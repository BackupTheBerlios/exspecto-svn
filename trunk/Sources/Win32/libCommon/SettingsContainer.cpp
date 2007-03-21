#include "SettingsContainer.h"
#include <algorithm>

//Инициализация статических переменных
Settings* Settings::m_pInstance = NULL;
std::string Settings::m_strModuleName = "";
char** Settings::m_pParamTypes = NULL;
int Settings::m_iParamCount = 0;

Settings::Settings()
{
	Log::instance().Trace( 100, "Settings::Settings: Загрузка параметров из settings.ini" );
	std::map< std::string, std::string > mapParamTypes;
	int iParam;
	unsigned int iTmp;
	char strBuffer[1024], strModName[1024];
	std::string strArg1, strArg2, strTmp;
	char *pFirstPos, *pSecondPos;

	for( int i = 0; i < m_iParamCount; i+=2 )
		mapParamTypes.insert( std::make_pair( m_pParamTypes[ i ], m_pParamTypes[ i + 1 ] ) );

	FILE* f = fopen( "settings.ini", "r" );
	if( NULL == f )
		throw ParamLoadErr( "Не удалось найти файл Settings.ini" );

	while( !feof(f) )
	{
		if( NULL == fgets( strBuffer, sizeof( strBuffer ), f ) )
		{
			if( feof(f) )
				break;
			
			throw ParamLoadErr( "Не удалось произвести чтение из файла Settings.ini" );
		}
			
		//Нашли новый раздел в файле параметров...
		if( ( pFirstPos = strstr( strBuffer, "[" ) ) && ( pSecondPos = strstr( strBuffer, "]" ) ) )
		{
			//Все следующие параметры будут считаться принадлежащими этому модулю
			memcpy( strModName, strBuffer + 1, pSecondPos - pFirstPos - 1 );
			strModName[ pSecondPos - pFirstPos - 1 ] = 0;
		}else
		{
			//Если параметры в файле принадлежат нашему модулю
			if( m_strModuleName == strModName )
			{
				strTmp = strBuffer;
				//Удаляем пробелы
				std::string::iterator itNewEnd = std::remove( strTmp.begin(), strTmp.end() - 1, ' ' );
				strTmp.resize( itNewEnd - strTmp.begin() );
				//Сохраняем параметр до знака = и после 
				if( ( iTmp = (int)strTmp.find( '=' ) ) != std::string::npos )
				{
					strArg1 = strTmp.substr( 0, iTmp );
					strArg2 = strTmp.substr( iTmp + 1 );
				}else
					continue;
				//Если это параметр есть в списке параметров
				if( mapParamTypes.find( strArg1 ) != mapParamTypes.end() )
				{
					//Выясняем тип параметра и записываем его в контейнер
					if( mapParamTypes[ strArg1 ] == "int" )
					{
						if( ( 0 == ( iParam = atoi( strArg2.c_str() ) ) && !strcmp( strArg2.c_str(), "0" ) ) )
						{
							strTmp += "Ошибка!Параметр ";
							strTmp += strArg1;
							strTmp += " должен быть целым числом";
							throw ParamLoadErr( strTmp );
						}
						PutParam( strArg1, iParam );
						Log::instance().Trace( 100, "Settings::Settings: Загружаем параметр %s = %d", strArg1.c_str(), iParam );
					}else if( mapParamTypes[ strArg1 ] == "string" )
					{
						Log::instance().Trace( 100, "Settings::Settings: Загружаем параметр %s = %s", strArg1.c_str(), strArg2.c_str() );
						PutParam( strArg1, strArg2 );
					}
				}
			}
		}
	}
	fclose(f);
	//Проверяем корректность заданных параметров
}
