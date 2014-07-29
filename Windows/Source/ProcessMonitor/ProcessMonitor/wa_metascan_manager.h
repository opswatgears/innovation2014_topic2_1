#pragma once

#include "wa_metascan_queue.h"
#include "wa_worker.h"
#include "wa_utils_json.h"
#include "wa_process_database.h"

enum WAMETASCAN_WORKER
{
	WAMETASCAN_REPORTER = 0,
	WAMETASCAN_CHECKER,
	WAMETASCAN_UPLOADER,
	WAMETASCAN_HASHLOADER,
	WAMETASCAN_TOTAL
};


class WaMetascanManager
{
public:
	
	bool start( std::wstring& s_key );
	TScanDetail* scan( std::wstring& path );

	WaMetascanManager( void );
	~WaMetascanManager(void);

private:	

	WaMetascanQueue< std::wstring >		m_pFileQueue;
	WaMetascanQueue< std::wstring >     m_pUpContentQueue;
	WaMetascanQueue< WaJson* >			m_pWaitQueue;
	WaMetascanQueue< WaJson* >			m_pResultQueue;

	WaProcessDatabase*					m_pDatabase;
	WaWorker*							m_pWorker[WAMETASCAN_TOTAL];
};

