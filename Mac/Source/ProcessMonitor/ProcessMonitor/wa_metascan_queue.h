//
//  wa_metascan_queue.h
//  TryLoader
//
//  Created by NTViet-MAC on 7/18/14.
//  Copyright (c) 2014 VietNguyen. All rights reserved.
//

#ifndef TryLoader_wa_metascan_queue_h
#define TryLoader_wa_metascan_queue_h

#include <pthread.h>
#include <queue>
#include "wa_utils_json.h"
using namespace std;

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
    
    queue<T>      m_queue;
    pthread_mutex_t m_tMutex;
};

// -- Implementation

template <class T>
WaMetascanQueue<T>::WaMetascanQueue()
{
    pthread_mutex_init( &( this->m_tMutex ), NULL );
}

template <class T>
WaMetascanQueue<T>::~WaMetascanQueue()
{
    pthread_mutex_destroy( &( this->m_tMutex ) );
}

template <class T>
void WaMetascanQueue<T>::put( T& in )
{
    pthread_mutex_lock( &( this->m_tMutex ) );
    this->m_queue.push( in );
    pthread_mutex_unlock( &( this->m_tMutex ) );
}

template <class T>
T WaMetascanQueue<T>::get()
{
    T out;
    
    pthread_mutex_lock( &( this->m_tMutex ) );
    if ( this->m_queue.size() > 0 )
    {
        out = this->m_queue.front();
        this->m_queue.pop();
    }
    pthread_mutex_unlock( &( this->m_tMutex ) );
    
    return out;
}

template <class T>
size_t WaMetascanQueue<T>::size()
{
    size_t size = 0;
    
    pthread_mutex_lock( &( this->m_tMutex ) );
    size = this->m_queue.size();
    pthread_mutex_unlock( &( this->m_tMutex ) );
    
    return size;
}

#endif
