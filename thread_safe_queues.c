//////////////////////////////////////////////////////////////////////////////////////
//                                                                                  //
//  Copyright (c) 2016-2017 Leonardo Consoni <consoni_2519@hotmail.com>             //
//                                                                                  //
//  This file is part of Simple Multithreading.                                     //
//                                                                                  //
//  Simple Multithreading is free software: you can redistribute it and/or modify   //
//  it under the terms of the GNU Lesser General Public License as published        //
//  by the Free Software Foundation, either version 3 of the License, or            //
//  (at your option) any later version.                                             //
//                                                                                  //
//  Simple Multithreading is distributed in the hope that it will be useful,        //
//  but WITHOUT ANY WARRANTY; without even the implied warranty of                  //
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the                    //
//  GNU Lesser General Public License for more details.                             //
//                                                                                  //
//  You should have received a copy of the GNU Lesser General Public License        //
//  along with Simple Multithreading. If not, see <http://www.gnu.org/licenses/>.   //
//                                                                                  //
//////////////////////////////////////////////////////////////////////////////////////


#include "thread_locks.h"
#include "semaphores.h"

#include "thread_safe_queues.h"

#include <string.h>
#include <stdlib.h>


struct _TSQueueData
{
  void** cache;
  size_t first, last, maxLength;
  size_t itemSize;
  TLock accessLock;
  Semaphore countLock;
};


TSQueue TSQ_Create( size_t maxLength, size_t itemSize )
{
  TSQueue queue = (TSQueue) malloc( sizeof(TSQueueData) );
  
  queue->maxLength = maxLength;
  queue->itemSize = itemSize;
  
  queue->cache = (void**) calloc( queue->maxLength, sizeof(void*) );
  for( size_t i = 0; i < queue->maxLength; i++ )
    queue->cache[ i ] = (void*) malloc( itemSize );
  
  queue->first = queue->last = 0;
  
  queue->accessLock = TLock_Create();
  queue->countLock = Sem_Create( 0, queue->maxLength );
  
  return queue;
}

void TSQ_Discard( TSQueue queue )
{
  if( queue != NULL )
  {
    for( size_t i = 0; i < queue->maxLength; i++ )
      free( queue->cache[ i ] );
    free( queue->cache );
    
    TLock_Discard( queue->accessLock );
    Sem_Discard( queue->countLock );

    free( queue );
    queue = NULL;
  }
}

size_t TSQ_GetItemsCount( TSQueue queue )
{
  return ( queue->last - queue->first );
}

bool TSQ_Enqueue( TSQueue queue, void* buffer, enum TSQueueAccessMode mode )
{
  static void* dataIn = NULL;

  if( buffer == NULL ) return false;
  
  if( mode == TSQUEUE_WAIT || TSQ_GetItemsCount( queue ) < queue->maxLength ) 
    Sem_Increment( queue->countLock );
  
  TLock_Acquire( queue->accessLock );
  dataIn = queue->cache[ queue->last % queue->maxLength ];
  memcpy( dataIn, buffer, queue->itemSize );
  if( TSQ_GetItemsCount( queue ) == queue->maxLength ) queue->first++;
  queue->last++;
  TLock_Release( queue->accessLock );

  return true;
}

bool TSQ_Dequeue( TSQueue queue, void* buffer, enum TSQueueAccessMode mode )
{
  static void* dataOut = NULL;
  
  if( buffer == NULL ) return false;

  if( mode == TSQUEUE_NOWAIT && TSQ_GetItemsCount( queue ) == 0 )
    return false;
  
  Sem_Decrement( queue->countLock );
  
  TLock_Acquire( queue->accessLock );
  dataOut = queue->cache[ queue->first % queue->maxLength ];
  buffer = memcpy( buffer, dataOut, queue->itemSize );
  queue->first++;
  TLock_Release( queue->accessLock );
  
  return true;
}
