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


/// @file thread_safe_lists.h
/// @brief Abstractions for thread safe list data structures
///
/// Lists that can be safely (no data races) accessed by different concurrent threads

#ifndef THREAD_SAFE_LISTS_H
#define THREAD_SAFE_LISTS_H

#include <stdint.h> 
#include <stdbool.h> 
#include <stddef.h>

/// Structure holding single thread safe list data
typedef struct _TSListData TSListData;
/// Opaque reference to thread safe list data structure
typedef TSListData* TSList;

                                                                         
/// @brief Creates new thread safe list data structure  
/// @param[in] itemSize size (in bytes) of created list items 
/// @return reference to newly created list data structure
TSList TSL_Create( size_t itemSize );

/// @brief Deallocates given thread safe list data structure
/// @param[in] list reference to list
void TSL_Discard( TSList list );

/// @brief Gets given thread safe list current number of stored items                             
/// @param[in] list reference to list
/// @return current number of stored items       
size_t TSL_GetItemsCount( TSList list );

/// @brief Copies given item to the end of given thread safe list                              
/// @param[in] list reference to list
/// @param[in] dataIn opaque pointer to inserted variable
/// @return newly created mutex reference
size_t TSL_Insert( TSList list, void* dataIn );

/// @brief Gets access key to item of given index                           
/// @param[in] list reference to list
/// @param[in] index current index of desired item
/// @return access key for desired item (-1 on errors)
int TSL_GetIndexKey( TSList list, size_t index );

/// @brief Removes item of given access key from the list                              
/// @param[in] list reference to list
/// @param[in] key access key of item to be removed
/// @return true on successful removal, false otherwise
bool TSL_Remove( TSList list, int key );

/// @brief Gets direct reference to item of given access key, locking the list (should be unlocked after)                              
/// @param[in] list reference to list
/// @param[in] key access key of item to be acquired
/// @return opaque pointer to desired item
void* TSL_AcquireItem( TSList list, int key );

/// @brief Unlocks lists (after previous aquisition lock)                             
/// @param[in] list reference to list
void TSL_ReleaseItem( TSList list );

/// @brief Copies item of specified access key from thread safe list to given buffer                       
/// @param[in] list reference to list
/// @param[in] key access key of item to be copied
/// @param[out] dataOut opaque pointer to preallocated buffer for variable
/// @return true on successful copy, false otherwise 
bool TSL_GetItem( TSList list, int key, void* dataOut );

/// @brief Copies given item to thread safe list, on specified access key location                            
/// @param[in] list reference to list
/// @param[in] key access key of item to be created/updated
/// @param[in] dataIn opaque pointer to copied variable
/// @return true on successful copy, false otherwise 
bool TSL_SetItem( TSList list, int key, void* dataIn );

#endif // THREAD_SAFE_LISTS_H
