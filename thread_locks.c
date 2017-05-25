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

#include "semaphores.h"

#ifdef WIN32

#include <Windows.h>

// Request new unique mutex for using in thread syncronization
ThreadLock ThreadLocks_Create()
{
  CRITICAL_SECTION* lock = malloc( sizeof(CRITICAL_SECTION) );
  InitializeCriticalSection( lock );
  return (ThreadLock) lock;
}

void ThreadLocks_Discard( ThreadLock lock )
{
  DeleteCriticalSection( (CRITICAL_SECTION*) lock );
  free( lock );
}

// Mutex aquisition and release
void ThreadLocks_Acquire( ThreadLock lock ) { EnterCriticalSection( (CRITICAL_SECTION*) lock ); }
void ThreadLocks_Release( ThreadLock lock ) { LeaveCriticalSection( (CRITICAL_SECTION*) lock ); }

#else // Unix

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <malloc.h>

// Request new unique mutex for using in thread syncronization
ThreadLock ThreadLocks_Create()
{
  pthread_mutex_t* newLock = (pthread_mutex_t*) malloc( sizeof(pthread_mutex_t) );
  pthread_mutex_init( newLock, NULL );
  return (ThreadLock) newLock;
}

void ThreadLocks_Discard( ThreadLock lock )
{
  pthread_mutex_destroy( (pthread_mutex_t*) lock );
  free( lock );
}

// Mutex aquisition and release
void ThreadLocks_Acquire( ThreadLock lock ) { pthread_mutex_lock( (pthread_mutex_t*) lock ); }
void ThreadLocks_Release( ThreadLock lock ) { pthread_mutex_unlock( (pthread_mutex_t*) lock ); }

#endif // WIN32
