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


#include "khash.h"

#include "thread_locks.h"

#include "thread_safe_maps.h"


typedef struct _MapItemData
{
  void* data;
  TLock accessLock;
}
MapItemData;

KHASH_MAP_INIT_INT64( RefInt, MapItemData )

struct _TSMapData
{
  khash_t( RefInt )* hashTable;
  enum TSMapKeyType keyType;
  size_t itemSize;
  TLock insertLock;
};


TSMap TSM_Create( enum TSMapKeyType keyType, size_t itemSize )
{
  TSMap newMap = (TSMap) malloc( sizeof(TSMapData) );
  
  newMap->hashTable = kh_init( RefInt );
  newMap->keyType = keyType;
  newMap->itemSize = itemSize;
  newMap->insertLock = TLock_Create();
  
  return newMap;
}

void TSM_Discard( TSMap map )
{
  if( map == NULL ) return;
  
  TLock_Acquire( map->insertLock );
  for( khint_t dataIndex = kh_begin( map->hashTable ); dataIndex < kh_end( map->hashTable ); dataIndex++ )
  {
    if( kh_exist( map->hashTable, dataIndex ) )
    {
      free( kh_value( map->hashTable, dataIndex ).data );
      TLock_Discard( kh_value( map->hashTable, dataIndex ).accessLock );
      kh_del( RefInt, map->hashTable, dataIndex );
    }
  }
  kh_destroy( RefInt, map->hashTable );
  TLock_Release( map->insertLock );
  
  TLock_Discard( map->insertLock );
  
  free( map );
}

size_t TSM_GetItemsCount( TSMap map )
{
  return kh_size( map->hashTable );
}

unsigned long TSM_SetItem( TSMap map, const void* key, void* dataIn )
{
  unsigned long hash = 0;
  int insertionStatus = 0;
  
  if( map->keyType == TSMAP_INT ) hash = (unsigned long) key;
  else if( key != NULL ) hash = (unsigned long) kh_str_hash_func( key );
  
  TLock_Acquire( map->insertLock );
  khint_t index = kh_get( RefInt, map->hashTable, hash );
  if( index == kh_end( map->hashTable ) ) 
  {
    index = kh_put( RefInt, map->hashTable, hash, &insertionStatus );
    kh_value( map->hashTable, index ).data = malloc( map->itemSize );
    kh_value( map->hashTable, index ).accessLock = TLock_Create();
  }
  TLock_Release( map->insertLock );
    
  TLock_Acquire( kh_value( map->hashTable, index ).accessLock );
  if( dataIn != NULL ) memcpy( kh_value( map->hashTable, index ).data, dataIn, map->itemSize );
  TLock_Release( kh_value( map->hashTable, index ).accessLock );
  
  if( insertionStatus == -1 ) hash = 0;
  else hash = (unsigned long) kh_key( map->hashTable, index );
  
  return hash;
}

bool TSM_RemoveItem( TSMap map, unsigned long hash )
{
  khint_t index = kh_get( RefInt, map->hashTable, (khint64_t) hash );
  if( index == kh_end( map->hashTable ) ) return false;
  
  TLock_Acquire( kh_value( map->hashTable, index ).accessLock );
  TLock_Release( kh_value( map->hashTable, index ).accessLock );
  
  free( kh_value( map->hashTable, index ).data );
  TLock_Discard( kh_value( map->hashTable, index ).accessLock );
  kh_del( RefInt, map->hashTable, index );
  
  return true;
}

void* TSM_AcquireItem( TSMap map, unsigned long hash )
{
  TLock_Acquire( map->insertLock );
  TLock_Release( map->insertLock );
  
  if( map == NULL ) return NULL;

  khint_t index = kh_get( RefInt, map->hashTable, (khint64_t) hash );
  if( index == kh_end( map->hashTable ) ) return NULL;
  
  TLock_Acquire( kh_value( map->hashTable, index ).accessLock );
  
  return kh_value( map->hashTable, index ).data;
}

void TSM_ReleaseItem( TSMap map, unsigned long hash )
{
  TLock_Acquire( map->insertLock );
  TLock_Release( map->insertLock );
  
  if( map == NULL ) return;
  
  khint_t index = kh_get( RefInt, map->hashTable, (khint64_t) hash );
  if( index == kh_end( map->hashTable ) ) return;
  
  TLock_Release( kh_value( map->hashTable, index ).accessLock );
}

bool TSM_GetItem( TSMap map, unsigned long hash, void* dataOut )
{
  void* ref_data = TSM_AcquireItem( map, hash );
  if( ref_data == NULL ) 
  {
    TSM_ReleaseItem( map, hash );
    return false;
  }
  
  if( dataOut != NULL ) memcpy( dataOut, ref_data, map->itemSize );
  
  TSM_ReleaseItem( map, hash );
  
  return true;
}

void TSM_RunForAllKeys( TSMap map, void (*objectOperator)( unsigned long ) )
{
  if( map == NULL ) return;
  
  for( khint_t dataIndex = kh_begin( map->hashTable ); dataIndex < kh_end( map->hashTable ); dataIndex++ )
  {
    if( kh_exist( map->hashTable, dataIndex ) )
      objectOperator( kh_key( map->hashTable, dataIndex ) );
    
    TLock_Acquire( map->insertLock );
    TLock_Release( map->insertLock );
  }
}
