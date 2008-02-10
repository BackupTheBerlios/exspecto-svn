#include "SettingsContainer.h"
#include <algorithm>
#include <list>
#include <vector>
#include <set>
#include <sstream>

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
  Log::instance().Trace( 100, "%s:  Settings.ini", __FUNCTION__ );
  std::map< std::string, std::string > mapParamTypes;
  unsigned int iTmp;
  char strBuffer[10240], strModName[1024];
  std::string strArg1, strArg2, strTmp;
  char *pFirstPos, *pSecondPos;

  for( int i = 0; i < iParamCount; i+=2 )
	mapParamTypes.insert( std::make_pair( pModuleParams[ i ], pModuleParams[ i + 1 ] ) );

  FILE* f = fopen( "settings.ini", "r" );
  if( NULL == f )
	throw ParamLoadErr( "error opening file Settings.ini" );

  while( !feof(f) )
	{
	  if( NULL == fgets( strBuffer, sizeof( strBuffer ), f ) )
		{
		  if( feof(f) )
			break;

		  throw ParamLoadErr( "error reading Settings.ini" );
		}

	  if( '/' == strBuffer[0] && '/' == strBuffer[1] )
		continue;
	  else 
		if( ( pFirstPos = strstr( strBuffer, "[" ) ) && ( pSecondPos = strstr( strBuffer, "]" ) ) )
		  {
			memcpy( strModName, strBuffer + 1, pSecondPos - pFirstPos - 1 );
			strModName[ pSecondPos - pFirstPos - 1 ] = 0;
		  }
		else if( strModuleName == strModName )
		  {
			strTmp = strBuffer;

			strTmp.erase( std::remove( strTmp.begin(), strTmp.end(), '\r' ), strTmp.end() );
			strTmp.erase( std::remove( strTmp.begin(), strTmp.end(), '\n' ), strTmp.end() );
			strTmp.erase( std::remove( strTmp.begin(), strTmp.end(), ' ' ), strTmp.end() );

			if( ( iTmp = (int)strTmp.find( '=' ) ) != std::string::npos )
			  {
				strArg1 = strTmp.substr( 0, iTmp );
				strArg2 = strTmp.substr( iTmp + 1 );
			  }
			else
			  continue;

			if( mapParamTypes.find( strArg1 ) != mapParamTypes.end() )
			  {
				if( m_mapSerializers.find( mapParamTypes[ strArg1 ] ) != m_mapSerializers.end() )
				  m_mapSerializers[ mapParamTypes[ strArg1 ] ]->Load( strArg1, strArg2, this );
				else
				  Log::instance().Trace( 10, "%s: Unknown parameter type %s: %s", __FUNCTION__, strArg1.c_str(), mapParamTypes[ strArg1 ].c_str() );
			  }
			else
			  Log::instance().Trace( 10, "%s: Unknown parameter %s", __FUNCTION__, strArg1.c_str() );
		  }
	}
  fclose(f);
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
  if( ( 0 == ( iParam = atoi( strParamValue.c_str() ) ) && ( 0 != strcmp( strParamValue.c_str(), "0" ) ) ) )
	{
	  Log::instance().Trace( 10, " %s: Parameter %s must be integer", __FUNCTION__, strParamName.c_str() );
	  throw ParamSerializeErr( strParamName );
	}else
	{
	  pParamContainer->PutParam( strParamName, iParam );
	  Log::instance().Trace( 100, "%s: Parameter loaded %s = %d", __FUNCTION__, strParamName.c_str(), iParam );
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
  Log::instance().Trace( 10, "%s: Parameter loaded %s = %s", __FUNCTION__, strParamName.c_str(), strParamValue.c_str() );
}

namespace{
  bool bStringReged = CIniSettings::RegisterTypeSerializer( "string", new CIniStringSerializer );
}

//-----------------------------------------------------------------------------------------------------------------
//---------------------------------------------CIniStringListSerializer--------------------------------------------
//-----------------------------------------------------------------------------------------------------------------

void CIniStringListSerializer::Load( const std::string& strParamName, const std::string& strParamValue, CSettings* pParamContainer )
{
  std::list< std::string > listStr;
  if( !Tools::GetStringList( strParamValue, listStr ) )
	{
	  Log::instance().Trace( 10, "%s: Parameter %s has incorrect format", __FUNCTION__, strParamName.c_str() );
	  throw ParamSerializeErr( strParamName );
	}
  pParamContainer->PutParam( strParamName, listStr );
  Log::instance().Trace( 100, "%s: Parameter loaded %s = %s", __FUNCTION__, strParamName.c_str(), strParamValue.c_str() );
}

namespace{
  bool bStringListReged = CIniSettings::RegisterTypeSerializer( "string-list", new CIniStringListSerializer );
}

//-----------------------------------------------------------------------------------------------------------------
//---------------------------------------------CIniStringListSerializer--------------------------------------------
//-----------------------------------------------------------------------------------------------------------------

void CIniBoolSerializer::Load( const std::string& strParamName, const std::string& strParamValue, CSettings* pParamContainer )
{
  bool bRes;
  if( "1" == strParamValue )
	bRes = true;
  else if( "0" == strParamValue )
	bRes = false;
  else
	{
	  Log::instance().Trace( 10, "%s: Parameter %s must be 0 or 1", __FUNCTION__, strParamName.c_str() );
	  throw ParamSerializeErr( strParamName );
	}
  pParamContainer->PutParam( strParamName, bRes );
  Log::instance().Trace( 100, "%s: Parameter loaded %s = %s", __FUNCTION__, strParamName.c_str(), strParamValue.c_str() );
}

namespace{
  bool bBoolReged = CIniSettings::RegisterTypeSerializer( "bool", new CIniBoolSerializer );
}

//-----------------------------------------------------------------------------------------------------------------
//---------------------------------------------CIniIpListSerializer------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------

void CIniIpListSerializer::Load( const std::string& strParamName, const std::string& strParamValue, CSettings* pParamContainer )
{

  Log::instance().Trace( 100, "%s: Parsing parameter %s = %s", __FUNCTION__, strParamName.c_str(), strParamValue.c_str() );

  std::vector< std::string > listIp;
  std::list< std::string > listStr;
  std::set< std::string > setExclude;

  if( !Tools::GetStringList( strParamValue, listStr ) )
	{
	  Log::instance().Trace( 0, "%s: Parameter %s has incorrect format", __FUNCTION__, strParamName.c_str() );		
	  throw ParamSerializeErr( strParamName );
	}
  for( std::list< std::string >::iterator It = listStr.begin(); It != listStr.end(); It++ )
	{
	  int iDelimPos = 0;
		
	  int iExclPos = -2;

	  if( std::string::npos == (size_t)( iDelimPos = (int)It->find( "-" ) ) 
		  && std::string::npos == (size_t)( iExclPos = (int)It->find( "!" ) ) )
		{
		  listIp.push_back( *It );
		}
	  else if( iExclPos > 0 )
		{
		  Log::instance().Trace( 5, "%s: !-symbol in incorrect place", __FUNCTION__ );
		}
	  else if( 0 == iExclPos )
		{
		  setExclude.insert( It->substr( 1 ) );
		}
	  else
		{
		  Tools::structIp Ip1, Ip2;
		  if( !Tools::ParseIp( It->substr( 0, iDelimPos ), Ip1 )
			  ||!Tools::ParseIp( It->substr( iDelimPos+1, It->size() - iDelimPos ), Ip2 ) )
			{
			  Log::instance().Trace( 10, "%s: Cannot pars %s: %s ", __FUNCTION__, strParamName.c_str(), It->c_str() );		
			  continue;
			}
		  std::string strTmpIp;
		  for( int iAInd = Ip1.A, iBInd = Ip1.B, iCInd = Ip1.C, iDInd = Ip1.D; iAInd <= Ip2.A; iAInd++ )
			{
			  std::stringstream ssA; ssA << iAInd;
			  for( ; ( (iAInd < Ip2.A) && (iBInd < 255) ) || ( (iAInd == Ip2.A) && (iBInd <= Ip2.B) ); iBInd++ )
				{
				  std::stringstream ssB; ssB << iBInd;
				  for( ; ( ( (iAInd < Ip2.A) || (iBInd < Ip2.B) ) && (iCInd < 255) ) || ( (iAInd == Ip2.A) && (iBInd == Ip2.B) && (iCInd <= Ip2.C) ); iCInd++ )
					{
					  std::stringstream ssC; ssC << iCInd;
					  for( ; ( ( (iAInd < Ip2.A) || (iBInd < Ip2.B) || (iCInd < Ip2.C) ) && (iDInd < 255) ) || ( (iAInd == Ip2.A) && (iBInd == Ip2.B) && (iCInd == Ip2.C) && (iDInd <= Ip2.D) ); iDInd++ )
						{
						  std::stringstream  ssD; ssD << iDInd;
						  strTmpIp.clear();
						  strTmpIp += ssA.str();
						  strTmpIp += ".";
						  strTmpIp += ssB.str();
						  strTmpIp += ".";
						  strTmpIp += ssC.str();
						  strTmpIp += ".";
						  strTmpIp += ssD.str();
						  listIp.push_back( strTmpIp );
						}
					  iDInd = 1;
					}
				  iCInd = 1;
				}
			  iBInd = 1;
			}
		}
	}
  std::vector< std::string >::iterator EraseIt;
  for( std::set< std::string >::iterator It = setExclude.begin(); It != setExclude.end(); It++ )
	{
		
	  if( listIp.end() != ( EraseIt = std::find( listIp.begin(), listIp.end(), *It ) ) )
		listIp.erase( EraseIt );
	}
  pParamContainer->PutParam( strParamName, listIp );
}

namespace{
  bool bIpListReged = CIniSettings::RegisterTypeSerializer( "ip-list", new CIniIpListSerializer );
}

//-----------------------------------------------------------------------------------------------------------------
//---------------------------------------------Tools---------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------

bool Tools::GetStringList( std::string strSource, std::list< std::string >& listDest )
{
  std::string strTmp;
  int iStartPos = 0, iEndPos = 0;

  strSource.erase( std::remove( strSource.begin(), strSource.end(), ' ' ), strSource.end() );

  while( (size_t)( iEndPos = (int)strSource.find( ',', iStartPos  ) ) != std::string::npos )
	{
	  strTmp = strSource.substr( iStartPos, iEndPos - iStartPos );
	  iStartPos = iEndPos + 1;
	  listDest.push_back( strTmp );
	}

  strTmp = strSource.substr( iStartPos );

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
