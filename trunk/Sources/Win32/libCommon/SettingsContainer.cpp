#include "SettingsContainer.h"
#include <algorithm>
#include <list>
#include <vector>

//-----------------------------------------------------------------------------------------------------------------
//---------------------------------------------CSettings-----------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------

CSettings::CSettings()
{
}

//-----------------------------------------------------------------------------------------------------------------
//---------------------------------------------CIniSettings--------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------

std::map< std::string, CIniTypeSerializer* > CIniSettings::m_mapSerializers;

CIniSettings::CIniSettings()
{
}

CIniSettings::~CIniSettings()
{
	for( std::map< std::string, CIniTypeSerializer* >::iterator It = m_mapSerializers.begin(); It != m_mapSerializers.end(); It++ )
		delete It->second;
}

void CIniSettings::SetModule( const std::string& strModuleName, char** pModuleParams, int iParamCount )
{
	Log::instance().Trace( 100, "CSettings::CSettings: �������� ���������� �� CSettings.ini" );
	std::map< std::string, std::string > mapParamTypes;
	unsigned int iTmp;
	char strBuffer[1024], strModName[1024];
	std::string strArg1, strArg2, strTmp;
	char *pFirstPos, *pSecondPos;

	for( int i = 0; i < iParamCount; i+=2 )
		mapParamTypes.insert( std::make_pair( pModuleParams[ i ], pModuleParams[ i + 1 ] ) );

	FILE* f = fopen( "Settings.ini", "r" );
	if( NULL == f )
		throw ParamLoadErr( "�� ������� ����� ���� Settings.ini" );

	while( !feof(f) )
	{
		if( NULL == fgets( strBuffer, sizeof( strBuffer ), f ) )
		{
			if( feof(f) )
				break;

			throw ParamLoadErr( "�� ������� ���������� ������ �� ����� CSettings.ini" );
		}

		//����� ����� ������ � ����� ����������...
		if( ( pFirstPos = strstr( strBuffer, "[" ) ) && ( pSecondPos = strstr( strBuffer, "]" ) ) )
		{
			//��� ��������� ��������� ����� ��������� �������������� ����� ������
			memcpy( strModName, strBuffer + 1, pSecondPos - pFirstPos - 1 );
			strModName[ pSecondPos - pFirstPos - 1 ] = 0;
			//���� ��������� � ����� ����������� ������ ������
		}else if( strModuleName == strModName )
		{
			strTmp = strBuffer;
			//������� �������
			strTmp.erase( std::remove( strTmp.begin(), strTmp.end() - 1, ' ' ), strTmp.end() );
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
				if( m_mapSerializers.find( mapParamTypes[ strArg1 ] ) != m_mapSerializers.end() )
					m_mapSerializers[ mapParamTypes[ strArg1 ] ]->Load( strArg1, strArg2, this );
				else
					Log::instance().Trace( 10, "CIniSettings::SetModule: ����������� ��� ��������� %s: %s", strArg1.c_str(), mapParamTypes[ strArg1 ].c_str() );
			}else
				Log::instance().Trace( 10, "CIniSettings::SetModule: ����������� �������� %s", strArg1.c_str() );
		}
	}
	fclose(f);
	//��������� ������������ �������� ����������
}

bool CIniSettings::RegisterTypeSerializer( const std::string& strType, CIniTypeSerializer* TypeSer )
{
	CIniSettings::m_mapSerializers[ strType ] = TypeSer;
	return true;
}

//-----------------------------------------------------------------------------------------------------------------
//---------------------------------------------CIniIntSerializer---------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------

void CIniIntSerializer::Load( const std::string& strParamName, const std::string& strParamValue, CSettings* pParamContainer )
{
	int iParam=0;
	std::string strTmp;
	if( ( 0 == ( iParam = atoi( strParamValue.c_str() ) ) && !strcmp( strParamValue.c_str(), "0" ) ) )
	{
		Log::instance().Trace( 10, " CIniIntSerializer::Load: �������� %s ������ ���� ����� ������ %s", strParamName.c_str() );
	}else
	{
		pParamContainer->PutParam( strParamName, iParam );
		Log::instance().Trace( 100, "CIniIntSerializer::Load: ��������� �������� %s = %d", strParamName.c_str(), iParam );
	}
}

namespace{
	bool bIntReged = CIniSettings::RegisterTypeSerializer( "int", new CIniIntSerializer );
}

//-----------------------------------------------------------------------------------------------------------------
//---------------------------------------------CIniStringSerializer------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------

void CIniStringSerializer::Load( const std::string& strParamName, const std::string& strParamValue, CSettings* pParamContainer )
{
	pParamContainer->PutParam( strParamName, strParamValue );
	Log::instance().Trace( 10, "CSettings::CSettings: ��������� �������� %s = %s", strParamName.c_str(), strParamValue.c_str() );
}

namespace{
	bool bStringReged = CIniSettings::RegisterTypeSerializer( "string", new CIniStringSerializer );
}

//-----------------------------------------------------------------------------------------------------------------
//---------------------------------------------CIniStringListSerializer--------------------------------------------
//-----------------------------------------------------------------------------------------------------------------

void CIniStringListSerializer::Load( const std::string& strParamName, const std::string& strParamValue, CSettings* pParamContainer )
{
	//���������� ������ �����
	std::list< std::string > listStr;
	if( !Tools::GetStringList( strParamValue, listStr ) )
		Log::instance().Trace( 10, "CIniStringListSerializer::Load: �������� %s ������ ���� ����� ������� �����", strParamName.c_str() );
	pParamContainer->PutParam( strParamName, listStr );
	Log::instance().Trace( 100, "CSettings::CSettings: ��������� �������� %s = %s", strParamName.c_str(), strParamValue.c_str() );
}

namespace{
	bool bStringListReged = CIniSettings::RegisterTypeSerializer( "string-list", new CIniStringListSerializer );
}

//-----------------------------------------------------------------------------------------------------------------
//---------------------------------------------CIniIpListSerializer------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------

void CIniIpListSerializer::Load( const std::string& strParamName, const std::string& strParamValue, CSettings* pParamContainer )
{
	//	���� ��� "-" - ��������� �����,����������
	//	����� ��������� �� ���������� ��� ������
	//	��� ������ ���������� A(���� a1<a2)
	//		��� ������ ���������� B(���� b1<b2 )
	//			��� ������ ���������� C(���� �1<c2)
	//				��� ������ ���������� D(���� d1<d2)
	//					������� �����

	Log::instance().Trace( 100, "CSettings::CSettings: ��������� �������� %s = %s", strParamName.c_str(), strParamValue.c_str() );
	std::vector< std::string > listIp;
	//��������� ������ ����������
	std::list< std::string > listStr;
	if( !Tools::GetStringList( strParamValue, listStr ) )
		Log::instance().Trace( 10, "CIniIpListSerializer::Load: �������� %s ������ ���� ����� ������� IP-�������", strParamName.c_str() );		
	//��� ������� ���������
	for( std::list< std::string >::iterator It = listStr.begin(); It != listStr.end(); It++ )
	{
		int iDelimPos = 0;
		
		if( std::string::npos == ( iDelimPos = (int)It->find( "-" ) ) )
		{
			listIp.push_back( *It );
		}else
		{
			Tools::structIp Ip1, Ip2;
			if( !Tools::ParseIp( It->substr( 0, iDelimPos ), Ip1 )
			  ||!Tools::ParseIp( It->substr( iDelimPos+1, It->size() - iDelimPos ), Ip2 ) )
			{
				Log::instance().Trace( 10, "CIniIpListSerializer::Load: ������ ��� ������� ��������� %s: %s.���������� ������ ", strParamName.c_str(), It->c_str() );		
				continue;
			}
			char strA[255], strB[255], strC[255], strD[255];
			std::string strTmpIp;
            for( int iAInd = Ip1.A; iAInd <= Ip2.A; iAInd++ )
			{
				itoa( iAInd, strA, 10 );
				for( int iBInd = Ip1.B; ( (iAInd < Ip2.A) && (iBInd < 255) ) || ( (iAInd == Ip2.A) && (iBInd <= Ip2.B) ); iBInd++ )
				{
					itoa( iBInd, strB, 10 );
					for( int iCInd = Ip1.C; ( (iBInd < Ip2.B) && (iCInd < 255) ) || ( (iBInd == Ip2.B) && (iCInd <= Ip2.C) ); iCInd++ )
					{
						itoa( iCInd, strC, 10 );
						for( int iDInd = Ip1.D; ( (iCInd < Ip2.C) && (iDInd < 255) ) || ( (iCInd == Ip2.C) && (iDInd <= Ip2.D) ); iDInd++ )
						{
                            itoa( iDInd, strD, 10 );
							strTmpIp.clear();
							strTmpIp += strA;
							strTmpIp += ".";
							strTmpIp += strB;
							strTmpIp += ".";
							strTmpIp += strC;
							strTmpIp += ".";
							strTmpIp += strD;
							listIp.push_back( strTmpIp );
						}
					}
				}
			}

			
		}
	}
	pParamContainer->PutParam( strParamName, listIp );
}

namespace{
	bool bIpListReged = CIniSettings::RegisterTypeSerializer( "ip-list", new CIniIpListSerializer );
}

//-----------------------------------------------------------------------------------------------------------------
//---------------------------------------------Tools---------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------

bool Tools::GetStringList( const std::string& strSource, std::list< std::string >& listDest )
{
	std::string strTmp;
	int iStartPos = 0, iEndPos = 0;
	while( ( iEndPos = (int)strSource.find( ',', iStartPos  ) ) != std::string::npos )
	{
		strTmp = strSource.substr( iStartPos, iEndPos );
		iStartPos = iEndPos + 1;
		//������� �������
		strTmp.erase( std::remove( strTmp.begin(), strTmp.end() - 1, ' ' ), strTmp.end() );
		listDest.push_back( strTmp );
	}
	//���������� �� �����
	strTmp = strSource.substr( iStartPos, strSource.size() );
	listDest.push_back( strTmp );
	return true;
}

bool Tools::ParseIp( const std::string& strSource, Tools::structIp& resIp )
{
	if( 3 != std::count( strSource.begin(), strSource.end(), '.' ) )
		return false;
	int iStartPos = 0, iEndPos = (int)strSource.find( '.' );
	resIp.A = atoi( strSource.substr( iStartPos, iEndPos - iStartPos ).c_str() );
	iStartPos = iEndPos;
	iEndPos = (int)strSource.find( '.', iEndPos + 1 );
	resIp.B = atoi( strSource.substr( iStartPos+1, iEndPos - iStartPos ).c_str() );
	iStartPos = iEndPos;
	iEndPos = (int)strSource.find( '.', iEndPos + 1 );
    resIp.C = atoi( strSource.substr( iStartPos+1, iEndPos - iStartPos ).c_str() );
	resIp.D = atoi( strSource.substr( iEndPos+1, strSource.size() - iEndPos ).c_str() );
	return true;
}