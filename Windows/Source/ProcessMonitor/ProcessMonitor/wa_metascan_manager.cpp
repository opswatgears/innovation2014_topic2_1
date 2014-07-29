#include "wa_metascan_manager.h"
#include "wa_metascan_uploader.h"
#include "wa_metascan_checker.h"
#include "wa_metascan_reporter.h"
#include "wa_metascan_hashloader.h"
#include "wa_metascan_namelookup.h"

WaMetascanManager::WaMetascanManager()
{		
	 WaMetascanNameLookup::initialize();	 
	 this->m_pDatabase = new WaProcessDatabase();
}


WaMetascanManager::~WaMetascanManager(void)
{
	for ( int i = 0 ; i < WAMETASCAN_TOTAL ; i++ )
	{
		if ( this->m_pWorker[i] != NULL )
		{
			this->m_pWorker[i]->stop();
			Sleep( 20 );
			delete this->m_pWorker[i];
			this->m_pWorker[i] = NULL;
		}
	}

	WaMetascanNameLookup::destroy();
	if ( this->m_pDatabase != NULL )
	{
		delete this->m_pDatabase;
	}
}

bool WaMetascanManager::start( std::wstring& s_key )
{
	this->m_pWorker[WAMETASCAN_REPORTER] = new WaMetascanReporter( &( m_pResultQueue ), this->m_pDatabase );
	this->m_pWorker[WAMETASCAN_CHECKER] = new WaMetascanChecker( s_key, &( this->m_pWaitQueue ) );
	this->m_pWorker[WAMETASCAN_UPLOADER] = new WaMetascanUploader( s_key, &( this->m_pUpContentQueue ) );
	this->m_pWorker[WAMETASCAN_HASHLOADER] = new WaMetascanHashLoader( s_key, &( this->m_pFileQueue ) );

	this->m_pWorker[WAMETASCAN_CHECKER]->setWorker(  &( this->m_pWorker[WAMETASCAN_REPORTER] ), 1 );

	WaWorker* p_temp1[] = { this->m_pWorker[WAMETASCAN_CHECKER], this->m_pWorker[WAMETASCAN_REPORTER] };
	this->m_pWorker[WAMETASCAN_UPLOADER]->setWorker( p_temp1, 2 );

	WaWorker* p_temp2[] = { this->m_pWorker[WAMETASCAN_UPLOADER], this->m_pWorker[WAMETASCAN_CHECKER], this->m_pWorker[WAMETASCAN_REPORTER] };
	this->m_pWorker[WAMETASCAN_HASHLOADER]->setWorker( p_temp2, 3 );

	for ( int i = 0 ; i < WAMETASCAN_TOTAL ; i++ )
	{
		if ( this->m_pWorker[i]->start() == false )
			return false;		 
	}

	return true;
}

TScanDetail* WaMetascanManager::scan( std::wstring& path )
{
	bool do_scan = false;
	TScanDetail* p_detail = this->m_pDatabase->get( path, do_scan );
	if ( do_scan )
	{
		this->m_pWorker[WAMETASCAN_HASHLOADER]->signal( ( void* )&path );
	}
	return p_detail;
}
