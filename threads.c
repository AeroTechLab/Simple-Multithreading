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

#include "threads.h"

#ifdef WIN32

#include <Windows.h>

// Setup new thread to run the given method asynchronously
Thread Thread_Start( AsyncFunction function, void* args, enum ThreadResourceMode mode )
{
  static HANDLE handle;
  static unsigned int threadID;
  
  if( (handle = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE) function, args, 0, &threadID )) == THREAD_INVALID_HANDLE )
  {
    perror( "Thread_Start: CreateThread: " );
    return THREAD_INVALID_HANDLE;
  }
  
  if( mode == THREAD_DETACHED ) CloseHandle( handle );

  return (Thread) handle;
}

// Wait for the thread of the given manipulator to exit and return its exiting value
uint32_t Thread_WaitExit( Thread handle, unsigned int milliseconds )
{
  static DWORD exitCode = 0;
  static DWORD exitStatus = WAIT_OBJECT_0;

  if( (HANDLE) handle != THREAD_INVALID_HANDLE )
  {
    exitStatus = WaitForSingleObject( handle, (DWORD) milliseconds );
  
    if( exitStatus == WAIT_FAILED || exitStatus == WAIT_TIMEOUT )
      perror( "Thread_WaitExit: WaitForSingleObject: " );
    
    GetExitCodeThread( handle, &exitCode );
  
    if( CloseHandle( (HANDLE) handle ) == 0 )
      perror( "Thread_WaitExit: CloseHandle: " );
  }

  return exitCode;
}

unsigned long Thread_GetID()
{
  return GetCurrentThreadId();
}

#else // Unix

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <malloc.h>

typedef struct _ThreadController
{
  pthread_t handle;
  pthread_cond_t condition;
  pthread_mutex_t lock;
  void* result;
} ThreadController;

// Setup new thread to run the given method asyncronously
Thread Thread_Start( void* (*function)( void* ), void* args, enum ThreadResourceMode mode )
{
  pthread_t* handle = (pthread_t*) malloc( sizeof(pthread_t) );
  
  if( pthread_create( handle, NULL, function, args ) != 0 )
  {
    perror( "Thread_Start: pthread_create: " );
    return THREAD_INVALID_HANDLE;
  }
  
  if( mode == THREAD_DETACHED ) pthread_detach( *handle );

  return (Thread) handle;
}

// Waiter function to be called asyncronously
static void* Waiter( void *args )
{
  ThreadController* controller = (ThreadController*) args;
    
  pthread_join( controller->handle, &(controller->result) );
  pthread_mutex_lock( &(controller->lock) );
  pthread_mutex_unlock( &(controller->lock) );
  pthread_cond_signal( &(controller->condition) );

  pthread_exit( NULL );
  return 0;
}

// Wait for the thread of the given manipulator to exit and return its exiting value
uint32_t Thread_WaitExit( Thread handle, unsigned int milliseconds )
{
  static struct timespec timeout;
  pthread_t controlHandle;
  ThreadController controlArgs = { .handle = *((pthread_t*) handle) };
  int controlResult;

  if( handle != THREAD_INVALID_HANDLE )
  {
    clock_gettime( CLOCK_REALTIME, &timeout );

    if( timeout.tv_sec + ( milliseconds / 1000 ) < INFINITE )
    {
      timeout.tv_sec += (time_t) ( milliseconds / 1000 );
      timeout.tv_nsec += (long) 1000000 * ( milliseconds % 1000 );
    }
  
    pthread_mutex_init( &(controlArgs.lock), 0 );
    pthread_cond_init( &(controlArgs.condition), 0 );
    pthread_mutex_lock( &(controlArgs.lock) );

    if( pthread_create( &controlHandle, NULL, Waiter, (void*) &controlArgs ) != 0 )
    {
      perror( "wait_thread_end: pthread_create: error creating waiter thread:" );
      return 0;
    }
  
    do controlResult = pthread_cond_timedwait( &(controlArgs.condition), &(controlArgs.lock), &timeout );
    while( controlArgs.result != NULL && controlResult != ETIMEDOUT );

    pthread_cancel( controlHandle );
    pthread_join( controlHandle, NULL );

    pthread_cond_destroy( &(controlArgs.condition) );
    pthread_mutex_destroy( &(controlArgs.lock) );
  
    if( controlArgs.result != NULL ) return *((uint32_t*) (controlArgs.result));
    
    free( handle );
  }

  return 0;
}

unsigned long Thread_GetID()
{
  return (unsigned long) pthread_self();
}

#endif // WIN32
