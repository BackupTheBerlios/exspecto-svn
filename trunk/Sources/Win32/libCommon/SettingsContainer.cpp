#include "SettingsContainer.h"
#include <algorithm>

//������������� ����������� ����������
Settings* Settings::m_pInstance = NULL;
std::string Settings::m_strModuleName = "";
char** Settings::m_pParamTypes = NULL;
int Settings::m_iParamCount = 0;

Settings::Settings()
{
	Log::instance().Trace( 100, "Settings::Settings: �������� ���������� �� settings.ini" );
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
		throw ParamLoadErr( "�� ������� ����� ���� Settings.ini" );

	while( !feof(f) )
	{
		if( NULL == fgets( strBuffer, sizeof( strBuffer ), f ) )
		{
			if( feof(f) )
				break;
			
			throw ParamLoadErr( "�� ������� ���������� ������ �� ����� Settings.ini" );
		}
			
		//����� ����� ������ � ����� ����������...
		if( ( pFirstPos = strstr( strBuffer, "[" ) ) && ( pSecondPos = strstr( strBuffer, "]" ) ) )
		{
			//��� ��������� ��������� ����� ��������� �������������� ����� ������
			memcpy( strModName, strBuffer + 1, pSecondPos - pFirstPos - 1 );
			strModName[ pSecondPos - pFirstPos - 1 ] = 0;
		}else
		{
			//���� ��������� � ����� ����������� ������ ������
			if( m_strModuleName == strModName )
			{
				strTmp = strBuffer;
				//������� �������
				std::string::iterator itNewEnd = std::remove( strTmp.begin(), strTmp.end() - 1, ' ' );
				strTmp.resize( itNewEnd - strTmp.begin() );
				//��������� �������� �� ����� = � ����� 
				if( ( iTmp = (int)strTmp.find( '=' ) ) != std::string::npos )
				{
					strArg1 = strTmp.substr( 0, iTmp );
					strArg2 = strTmp.substr( iTmp + 1 );
				}else
					continue;
				//���� ��� �������� ���� � ������ ����������
				if( mapParamTypes.find( strArg1 ) != mapParamTypes.end() )
				{
					//�������� ��� ��������� � ���������� ��� � ���������
					if( mapParamTypes[ strArg1 ] == "int" )
					{
						if( ( 0 == ( iParam = atoi( strArg2.c_str() ) ) && !strcmp( strArg2.c_str(), "0" ) ) )
						{
							strTmp += "������!�������� ";
							strTmp += strArg1;
							strTmp += " ������ ���� ����� ������";
							throw ParamLoadErr( strTmp );
						}
						PutParam( strArg1, iParam );
						Log::instance().Trace( 100, "Settings::Settings: ��������� �������� %s = %d", strArg1.c_str(), iParam );
					}else if( mapParamTypes[ strArg1 ] == "string" )
					{
						Log::instance().Trace( 100, "Settings::Settings: ��������� �������� %s = %s", strArg1.c_str(), strArg2.c_str() );
						PutParam( strArg1, strArg2 );
					}
				}
			}
		}
	}
	fclose(f);
	//��������� ������������ �������� ����������
}
