#pragma once

#include <queue>
#include <Windows.h>

template <class T>
class WaMetascanQueue
{
public:
	WaMetascanQueue();
	~WaMetascanQueue();

	void    put( T& in );
	T       get();
	size_t  size();

private:

	std::queue<T>		m_queue;
	CRITICAL_SECTION	m_tCriticalSection;
};

// -- Implementation
//
template <class T>
WaMetascanQueue<T>::WaMetascanQueue()
{
	::InitializeCriticalSection( &( this->m_tCriticalSection ) );
}

template <class T>
WaMetascanQueue<T>::~WaMetascanQueue()
{
	::DeleteCriticalSection( &( this->m_tCriticalSection ) );
}

template <class T>
void WaMetascanQueue<T>::put( T& in )
{
	EnterCriticalSection( &( this->m_tCriticalSection ) );
	this->m_queue.push( in );
	LeaveCriticalSection( &( this->m_tCriticalSection ) );
}

template <class T>
T WaMetascanQueue<T>::get()
{
	T out;

	EnterCriticalSection( &( this->m_tCriticalSection ) );
	if ( this->m_queue.size() > 0 )
	{
		out = this->m_queue.front();
		this->m_queue.pop();
	}
	LeaveCriticalSection( &( this->m_tCriticalSection ) );

	return out;
}

template <class T>
size_t WaMetascanQueue<T>::size()
{
	size_t size = 0;

	EnterCriticalSection( &( this->m_tCriticalSection ) );
	size = this->m_queue.size();
	LeaveCriticalSection( &( this->m_tCriticalSection ) );

	return size;
}