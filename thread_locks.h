////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  Copyright (c) 2016-2017 Leonardo Consoni <consoni_2519@hotmail.com>       //
//                                                                            //
//  This file is part of Platform Utils.                                      //
//                                                                            //
//  Platform Utils is free software: you can redistribute it and/or modify    //
//  it under the terms of the GNU Lesser General Public License as published  //
//  by the Free Software Foundation, either version 3 of the License, or      //
//  (at your option) any later version.                                       //
//                                                                            //
//  Platform Utils is distributed in the hope that it will be useful,         //
//  but WITHOUT ANY WARRANTY; without even the implied warranty of            //
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the              //
//  GNU Lesser General Public License for more details.                       //
//                                                                            //
//  You should have received a copy of the GNU Lesser General Public License  //
//  along with Platform Utils. If not, see <http://www.gnu.org/licenses/>.    //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


/// @file threads/threading.h
/// @brief Platform agnostic multithreading functions.
///
/// Wrapper library for thread management (creation and syncronization)
/// abstracting underlying low level operating system native calls                    

#ifndef THREADING_H
#define THREADING_H

#include <stdint.h>

#include "namespaces.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////
/////                                      THREADS HANDLING                                       /////
///////////////////////////////////////////////////////////////////////////////////////////////////////

#define THREAD_INVALID_HANDLE NULL            ///< Handle to be returned on thread creation errors
#ifndef INFINITE
  #define INFINITE 0xFFFFFFFF                 ///< Infinite waiting time
#endif

/// Controls the thread creation and resource management mode
enum ThreadResourceMode
{ 
       THREAD_DETACHED,      ///< Thread termination won't be awaited on any point of the code (resources are freed independently)
       THREAD_JOINABLE       ///< Thread termination will be awaited (for synchornization) with WaitExit() before its allocated resources are freed 
};
       
typedef void* Thread;                       ///< Opaque alias for platform specific thread handle type
typedef void* (*AsyncFunction)( void* );    ///< Signature/type required for functions offloaded to another thread

/// Thread management interface generation macro
#define THREAD_INTERFACE( Namespace, INIT_FUNCTION ) \
        INIT_FUNCTION( Thread, Namespace, StartThread, AsyncFunction, void*, enum ThreadResourceMode ) \
        INIT_FUNCTION( uint32_t, Namespace, WaitExit, Thread, unsigned int ) \
        INIT_FUNCTION( unsigned long, Namespace, GetCurrentThreadID, void )

DECLARE_NAMESPACE_INTERFACE( Threading, THREAD_INTERFACE );

#define THREAD_ID Threading.GetCurrentThreadID()    ///< Alias for getting calling thread platform specific identifier



typedef void* ThreadLock;       ///< Opaque alias for platform specific thread multiplexer (mutex) type

/// Thread mutex interface generation macro
#define THREAD_LOCK_INTERFACE( Namespace, INIT_FUNCTION ) \
        INIT_FUNCTION( ThreadLock, Namespace, Create, void ) \
        INIT_FUNCTION( void, Namespace, Discard, ThreadLock ) \
        INIT_FUNCTION( void, Namespace, Acquire, ThreadLock ) \
        INIT_FUNCTION( void, Namespace, Release, ThreadLock )

DECLARE_NAMESPACE_INTERFACE( ThreadLocks, THREAD_LOCK_INTERFACE );



typedef struct _SemaphoreData SemaphoreData;    ///< Data structure to hold single semaphore data
typedef SemaphoreData* Semaphore;               ///< Opaque type to semaphore data structure

/// Semaphore interface generation macro
#define SEMAPHORE_INTERFACE( Namespace, INIT_FUNCTION ) \
        INIT_FUNCTION( Semaphore, Namespace, Create, size_t, size_t ) \
        INIT_FUNCTION( void, Namespace, Discard, Semaphore ) \
        INIT_FUNCTION( void, Namespace, Increment, Semaphore ) \
        INIT_FUNCTION( void, Namespace, Decrement, Semaphore ) \
        INIT_FUNCTION( size_t, Namespace, GetCount, Semaphore ) \
        INIT_FUNCTION( void, Namespace, SetCount, Semaphore, size_t )

DECLARE_NAMESPACE_INTERFACE( Semaphores, SEMAPHORE_INTERFACE );


#endif // THREADING_H


/// @struct Threading
/// @brief Function pointer struct as multithreading interface  
///
/// @memberof Threading
/// @fn Thread StartThread( void* (*function)( void* ), void* args, enum ThreadResourceMode mode )                                                                              
/// @brief Setups new thread to run the given method asynchronously                                               
/// @param[in] function pointer to the function that will run on spawned thread                                   
/// @param[in] args opaque pointer to struct that will be passed as the thread function argument                                         
/// @param[in] mode resources management option (THREAD_DETACHED or THREAD_JOINABLE)       
/// @return Handle to the newly created thread (THREAD_INVALID_HANDLE on errors)  
///
/// @memberof Threading
/// @fn uint32_t WaitExit( Thread handle, unsigned int milliseconds )
/// @brief Waits for the thread of the given manipulator to exit and return its exiting value                              
/// @param[in] handle handle to a joinable (started with THREAD_JOINABLE) thread
/// @param[in] milliseconds timeout (in milliseconds) for thread termination waiting (INFINITE to wait indefinitely)       
/// @return return value of given thread casted to unsigned int (0 on errors, detached thread, or timeout reached)
///
/// @memberof Threading
/// @fn unsigned long GetCurrentThreadID()
/// @brief Gets calling thread platform specific identifier                              
/// @return calling thread platform specific identifier 
///
/// @memberof Threading

/// @struct ThreadLocks
/// @brief Function pointer struct as thread locking interface  
///
/// @memberof ThreadLocks
/// @fn ThreadLock Create()                                                                             
/// @brief Request new unique mutex for using in thread syncronization                                                       
/// @return newly created mutex reference
///
/// @memberof ThreadLocks
/// @fn void Discard( ThreadLock lock )
/// @brief Discards given mutex data                              
/// @param[in] lock mutex reference
///
/// @memberof ThreadLocks
/// @fn void Acquire( ThreadLock lock )
/// @brief Mutex acquisition (blocks calling thread if it is already acquired in another thread)                              
/// @param[in] lock mutex reference
///
/// @memberof ThreadLocks
/// @fn void Release( ThreadLock lock )
/// @brief Mutex release (should always be done after acquiring to awake other threads locking it)                              
/// @param[in] lock mutex reference
///
/// @memberof ThreadLocks

/// @struct Semaphores
/// @brief Function pointer struct as semaphores interface  
///
/// @memberof Semaphores
/// @fn Semaphore Create( size_t startCount, size_t maxCount )                                                                              
/// @brief Creates semaphore object/data structure and initializes it                                                
/// @param[in] startCount starting value/count for newly created semaphore                                
/// @param[in] maxCount maximum allowed value/count of newly created semaphore                                             
/// @return reference to newly created semaphore data structure  
///
/// @memberof Semaphores
/// @fn void Discard( Semaphore sem )
/// @brief Discards given semaphore data structure                            
/// @param[in] sem reference to semaphore data structure
///
/// @memberof Semaphores
/// @fn void Increment( Semaphore sem )
/// @brief Increases internal count for given semaphore, blocking thread if maximum count is reached                             
/// @param[in] sem reference to semaphore data structure
///
/// @memberof Semaphores
/// @fn void Decrement( Semaphore sem )
/// @brief Decreases internal count for given semaphore, blocking thread if zero count is reached                    
/// @param[in] sem reference to semaphore data structure
///
/// @memberof Semaphores
/// @fn size_t GetCount( Semaphore sem )
/// @brief Reads current internal count for given semaphore                              
/// @param[in] sem reference to semaphore data structure
/// @return current internal count 
///
/// @memberof Semaphores
/// @fn void SetCount( Semaphore sem, size_t count )
/// @brief Defines desired internal count for given semaphore                              
/// @param[in] sem reference to semaphore data structure
/// @param[in] count desired internal count (should be between 0 and the maximum count) 
///
/// @memberof Semaphores
