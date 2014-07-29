#pragma once
#include "wa_worker.h"
#include "wa_metascan_queue.h"
#include "wa_process_database.h"

class WaMetascanReporter: public WaWorker
{
public:
	WaMetascanReporter( WaMetascanQueue< WaJson* >* p_result_queue, WaProcessDatabase* p_database );
	virtual ~WaMetascanReporter(void);

	virtual bool start();

private:

	static DWORD WINAPI _startWorking( LPVOID lpParam );
	void				_reportResult();
	void				_processSignal( void* p_data );

private:

	WaMetascanQueue< WaJson* >* m_pResultQueue;
	WaProcessDatabase*	m_pDatabase;
};

