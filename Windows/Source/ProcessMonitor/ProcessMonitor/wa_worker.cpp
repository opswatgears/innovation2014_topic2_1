#include "wa_worker.h"


WaWorker::WaWorker(void)
{
	for ( int i = 0 ; i <WA_WORKER_EVENT_TOTAL ; i++ )
	{
		this->m_hEvents[i] = CreateEvent( NULL, TRUE, FALSE, NULL );
	}
}


WaWorker::~WaWorker(void)
{
	for ( int i = 0 ; i <WA_WORKER_EVENT_TOTAL ; i++ )
	{
		if ( this->m_hEvents[i] != NULL )
		{
			CloseHandle( this->m_hEvents[i] );
			this->m_hEvents[i] = NULL;
		}
	}

	delete[] this->m_pWorker;
}

void WaWorker::stop()
{
	ResetEvent( this->m_hEvents[WA_WORKER_EVENT_DO] );
	SetEvent( this->m_hEvents[WA_WORKER_EVENT_STOP] );
}

void WaWorker::signal( void* p_data )
{
	this->_processSignal( p_data );
	SetEvent( this->m_hEvents[WA_WORKER_EVENT_DO] );
}

void WaWorker::setWorker( WaWorker** p_worker, int n_worker )
{
	this->m_nWorker = n_worker;
	this->m_pWorker = new WaWorker* [this->m_nWorker];
	for ( int i = 0 ; i < this->m_nWorker ; i++ )
	{
		this->m_pWorker[i] = p_worker[i];
	}
}