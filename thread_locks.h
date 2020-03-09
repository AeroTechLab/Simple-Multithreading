//////////////////////////////////////////////////////////////////////////////////////
//                                                                                  //
//  Copyright (c) 2016-2020 Leonardo Consoni <leonardojc@protonmail.com>            //
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


/// @file thread_locks.h
/// @brief Platform agnostic mutex/lock functions.
///
/// Wrapper library for thread synchronization (using mutexes/locks)
/// abstracting underlying low level operating system native calls                    

#ifndef THREAD_LOCKS_H
#define THREAD_LOCKS_H

typedef void* TLock;       ///< Opaque alias for platform specific thread multiplexer (mutex) type

                                                                     
/// @brief Request new unique mutex for using in thread syncronization                                                       
/// @return newly created mutex reference
TLock TLock_Create();

/// @brief Discards given mutex data                              
/// @param[in] lock mutex reference
void TLock_Discard( TLock lock );

/// @brief Mutex acquisition (blocks calling thread if it is already acquired in another thread)                              
/// @param[in] lock mutex reference
void TLock_Acquire( TLock lock );

/// @brief Mutex release (should always be done after acquiring to awake other threads locking it)                              
/// @param[in] lock mutex reference
void TLock_Release( TLock lock );


#endif // THREAD_LOCKS_H
