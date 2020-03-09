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


/// @file thread_safe_maps.h
/// @brief Abstractions for thread safe map data structures
///
/// Maps/dictionaries/hash tables that can be safely (no data races) accessed by different concurrent threads

#ifndef THREAD_SAFE_MAPS_H
#define THREAD_SAFE_MAPS_H

#include <stdint.h> 
#include <stdbool.h> 


/// Type of key used by created map
enum TSMapKeyType 
{ 
  TSMAP_INT,              ///< Use integer keys
  TSMAP_STR               ///< Use string keys
};

/// Structure holding single thread safe map data
typedef struct _TSMapData TSMapData;
/// Opaque reference to thread safe map data structure
typedef TSMapData* TSMap;

                                                                          
/// @brief Creates new thread safe queue map structure                                                  
/// @param[in] keyType type of indexation key used (TSMAP_INT or TSMAP_STR)                                
/// @param[in] itemSize size (in bytes) of created map items                                             
/// @return reference to newly created map data structure  
TSMap TSM_Create( enum TSMapKeyType keyType, size_t itemSize );

/// @brief Deallocates given thread safe queue map structure                        
/// @param[in] map reference to map
void TSM_Discard( TSMap map );

/// @brief Gets given thread safe map current number of stored items                               
/// @param[in] map reference to map
/// @return current number of stored items      
size_t TSM_GetItemsCount( TSMap map );

/// @brief Inserts new item (key/value pair) on given thread safe map                 
/// @param[in] map reference to map
/// @param[in] key opaque pointer to integer or string (depending on map's type) associated with inserted value
/// @param[in] dataIn opaque pointer to copied variable
/// @return unique hash identifier to inserted item (0 on errors)
unsigned long TSM_SetItem( TSMap map, const void* key, void* dataIn );

/// @brief Removes item of given hash identifier from the list                         
/// @param[in] map reference to map
/// @param[in] hash identifier of removed item 
/// @return true on succesful removal, false otherwise
bool TSM_RemoveItem( TSMap map, unsigned long hash );

/// @brief Gets direct reference to value of given item, locking its access (should be unlocked afterwards)                              
/// @param[in] map reference to map
/// @param[in] hash identifier of acquired item  
void* TSM_AcquireItem( TSMap map, unsigned long hash );

/// @brief Unlocks access for given item                           
/// @param[in] map reference to map
/// @param[in] hash identifier of released item 
void TSM_ReleaseItem( TSMap map, unsigned long hash );

/// @brief Copies item of specified hash identifier from thread safe list to given buffer                              
/// @param[in] map reference to map
/// @param[in] hash identifier of copied item 
/// @param[out] dataOut opaque pointer to preallocated buffer for variable
/// @return true on successful copy, false otherwise 
bool TSM_GetItem( TSMap map, unsigned long hash, void* dataOut );

/// @brief Applies function to all current elements of given thread safe map                              
/// @param[in] map reference to map
/// @param[in] objectOperator pointer of function to be applied to all elements, takes hash identifier as argument 
void TSM_RunForAllKeys( TSMap map, void (*objectOperator)( unsigned long ) );

#endif // THREAD_SAFE_MAPS_H
