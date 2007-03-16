#include "CTask.h"

enumAgentState CTask::m_CurState = Idling;
CCriticalSection CTask::m_csCurState;
CEvent CTask::m_CancelEv(false);
std::vector< std::string > CTask::m_vecData;
Container< CScanner*, PluginLoadStrategy > CStartScan::m_PluginContainer;

//-----------------------------------------------------------------------------------------------------------------
//---------------------------------------------CGetStatus----------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------

bool CGetStatus::Immidiate()
{
	m_csCurState.Enter();
		BYTE bResp[] = { RESP_OK, m_CurState }; 
		//m_pSocket->Send( (void*)&bResp, sizeof( bResp ) );
		Log::instance().Dump( 90, bResp, sizeof( bResp ), "CGetStatus:Immidiate: ��������� �����:" );
	m_csCurState.Leave();
	return true;
}

//-----------------------------------------------------------------------------------------------------------------
//---------------------------------------------CStartScan----------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------
	
bool CStartScan::Immidiate()
{
	BYTE bResp[] = { RESP_OK };
	//m_pSocket->Send( (void*)&bResp, sizeof( bResp ) );
	Log::instance().Dump( 90, bResp, sizeof( bResp ), "CStartScan:Immidiate: ��������� �����:" );
	return false;
}
	
void CStartScan::Execute()
{
	m_vecData.clear();
	Log::instance().Trace( 90, "CStartScan: �������� ������ �� ������ ������������" );
	m_csCurState.Enter();
		m_CurState = Scanning;	
	m_csCurState.Leave();
/*
	for( std::vector< std::string >::iterator AddrIt = m_vecAddresses.begin(); AddrIt != m_vecAddresses.end(); AddrIt++ )
	{
		for( PluginIterator PlugIt = m_PluginContainer.begin(); PlugIt != m_PluginContainer.end(); PlugIt++ )
		{
			if( WAIT_OBJECT_0 == WaitForSingleObject( m_CancelEv, 0 ) )
				break;
			Log::instance().Trace( 80, "CStartScan: ��������� ����� %s � ������� ������� %s", AddrIt->c_str(), (*PlugIt)->GetProtocolName() );
			(*PlugIt)->Scan( *AddrIt, m_vecData, m_CancelEv );
		}
		if( WAIT_OBJECT_0 == WaitForSingleObject( m_CancelEv, 0 ) )
		{
			Log::instance().Trace( 90, "CStartScan: ������������ ��������" );
			//���������� ������� ������
			m_CancelEv.Reset();
			break;
		}
	}*/
	//TODO:��������,�������� ������� ������������ ������ �������� ����������,���� ���� ���������� 
	m_csCurState.Enter();
		m_CurState = Idling;		
	m_csCurState.Leave();
}

//-----------------------------------------------------------------------------------------------------------------
//---------------------------------------------CStopScan-----------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------

bool CStopScan::Immidiate()
{
	Log::instance().Trace( 90, "CStopScan: �������� ������ �� ������ ������������" );
	m_csCurState.Enter();
		if( Scanning == m_CurState )
		{
			Log::instance().Trace( 90, "CStopScan: �������� ������� ������������" );
			Cancel();
		}else
			Log::instance().Trace( 90, "CStopScan: � ������ ������ �� ��������� � ��������� ������������" );
	m_csCurState.Leave();
	BYTE bResp[] = { RESP_OK };	
	//m_pSocket->Send( (void*)&bResp, sizeof( bResp ) );
	Log::instance().Dump( 90, bResp, sizeof( bResp ), "CStopScan:Immidiate: ��������� �����:" );
	return true;
}

//-----------------------------------------------------------------------------------------------------------------
//---------------------------------------------CGetData------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------

bool CGetData::Immidiate()
{
	Log::instance().Trace( 90, "CGetData: �������� ������ �� ��������� ������" );
	//������������ ������ ������ ��� ��������
	int iSize = 0;
	for( std::vector< std::string >::iterator It = m_vecData.begin(); It != m_vecData.end(); It++ )
	{
		//+1 - �� ����������� ����
		iSize += It->size() + 1;
	}
	//4 ����� �� ������ 1 ���� - ��������� ��������� �������
	iSize += 4 + 1; 
	std::auto_ptr< BYTE > pbBuf = std::auto_ptr< BYTE >( new BYTE[ iSize ] );
	pbBuf.get()[0] = (BYTE)RESP_OK;
	::memcpy( pbBuf .get() + 1, (void*)&iSize, 4 );
	int iOffset = 5; 
	for( std::vector< std::string >::iterator It = m_vecData.begin(); It != m_vecData.end(); It++ )
	{
		strcpy( (char*)pbBuf.get() + iOffset, It->c_str() );
		iOffset += It->size() + 1;
	}
	//m_pSocket->Send( pbBuf.get(), iSize );
	Log::instance().Dump( 90, pbBuf.get(), iSize, "CGetData:Immidiate: ��������� �����:" );
	return true;
}
