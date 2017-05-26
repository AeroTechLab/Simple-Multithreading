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


/// @file thread_safe_queues.h
/// @brief Abstractions for thread safe queue data structures
///
/// Queues that can be safely (no data races) accessed by different concurrent threads

#ifndef THREAD_SAFE_QUEUES_H
#define THREAD_SAFE_QUEUES_H

#include <stdint.h> 
#include <stdbool.h> 
#include <stddef.h>

/// Structure holding single thread safe queue data
typedef struct _TSQueueData TSQueueData;
/// Opaque reference to thread safe queue data structure
typedef TSQueueData* TSQueue;

/// Option to control behaviour of queue access on empty or full cases
enum TSQueueAccessMode 
{ 
  TSQUEUE_WAIT,               ///< Wait for availability of data for reading or space for writing 
  TSQUEUE_NOWAIT              ///< Automatically return when reading empty queue or overwrite when writing to full queue
};

                                                                    
/// @brief Creates new thread safe queue data structure                                               
/// @param[in] maxLength maximum queue lenght (number of items)                                   
/// @param[in] itemSize size (in bytes) of created queue items                                               
/// @return reference to newly created queue data structure
TSQueue TSQ_Create( size_t maxLength, size_t itemSize );

/// @brief Deallocates given thread safe queue data structure                            
/// @param[in] queue reference to queue
void TSQ_Discard( TSQueue queue );

/// @brief Gets given thread safe queue current number of stored items
/// @param[in] queue reference to queue
/// @return current number of stored items
size_t TSQ_GetItemsCount( TSQueue queue );

/// @brief Copies given item to the end of given thread safe queue
/// @param[in] queue reference to queue
/// @param[in] buffer opaque pointer to inserted variable
/// @param[in] mode insertion behaviour (TSQUEUE_WAIT or TSQUEUE_NOWAIT)
/// @return true on successful copy/insertion, false otherwise 
bool TSQ_Enqueue( TSQueue queue, void* buffer, enum TSQueueAccessMode mode );

/// @brief Copies first item of the thread safe queue to given buffer and removes it from queue
/// @param[in] queue reference to queue
/// @param[out] buffer opaque pointer to preallocated buffer for variable
/// @param[in] mode removal behaviour (TSQUEUE_WAIT or TSQUEUE_NOWAIT)
/// @return true on successful copy/removal, false otherwise 
bool TSQ_Dequeue( TSQueue queue, void* buffer, enum TSQueueAccessMode mode );


#endif // THREAD_SAFE_QUEUES_H
