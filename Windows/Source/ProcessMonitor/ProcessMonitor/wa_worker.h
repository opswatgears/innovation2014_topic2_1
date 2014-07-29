#pragma once

#include <Windows.h>

enum WA_WORKER_EVENT{
	WA_WORKER_EVENT_DO = 0,
	WA_WORKER_EVENT_STOP,
	WA_WORKER_EVENT_TOTAL
};

class WaWorker
{
public:
	WaWorker(void);
	virtual ~WaWorker(void);

	virtual bool start() = 0;
	virtual void stop();
	void signal( void* p_data );
	void setWorker( WaWorker** p_worker, int n_worker );

protected:

	virtual void _processSignal( void* p_data ) {};

protected:

	HANDLE		m_hEvents[WA_WORKER_EVENT_TOTAL];
	int			m_nWorker;
	WaWorker**	m_pWorker;
};

