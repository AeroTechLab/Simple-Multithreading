//////////////////////////////////////////////////////////////////////////////////////
//                                                                                  //
//  Copyright (c) 2016-2025 Leonardo Consoni <leonardojc@protonmail.com>             //
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


/// @file threads.h
/// @brief Platform agnostic thread management functions.
///
/// Wrapper library for thread management (creation and syncronization)
/// abstracting underlying low level operating system native calls                    

#ifndef THREADS_H
#define THREADS_H

#include <stdint.h>
#include <stddef.h>

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

                                                                            
/// @brief Setups new thread to run the given method asynchronously                                               
/// @param[in] function pointer to the function that will run on spawned thread                                   
/// @param[in] args opaque pointer to struct that will be passed as the thread function argument                                         
/// @param[in] mode resources management option (THREAD_DETACHED or THREAD_JOINABLE)       
/// @return Handle to the newly created thread (THREAD_INVALID_HANDLE on errors)  
Thread Thread_Start( void* (*function)( void* ), void* args, enum ThreadResourceMode mode );

/// @brief Waits for the thread of the given manipulator to exit and return its exiting value                              
/// @param[in] handle handle to a joinable (started with THREAD_JOINABLE) thread
/// @param[in] milliseconds timeout (in milliseconds) for thread termination waiting (INFINITE to wait indefinitely)       
/// @return return value of given thread casted to unsigned int (0 on errors, detached thread, or timeout reached)
uint32_t Thread_WaitExit( Thread handle, unsigned int milliseconds );

/// @brief Gets calling thread platform specific identifier                              
/// @return calling thread platform specific identifier 
unsigned long Thread_GetID();

#endif // THREADS_H
