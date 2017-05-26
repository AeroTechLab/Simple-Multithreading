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

#include "thread_safe_lists.h"

#include <stdlib.h>
#include <string.h>

static const size_t LIST_LENGTH_INCREMENT = 10;

typedef struct _Item
{
  int key;
  void* data;
}
Item;

struct _TSListData
{
  Item* data;
  size_t length, itemsCount, insertCount;
  size_t itemSize;
  TLock accessLock;
};


TSList TSL_Create( size_t itemSize )
{
  TSList list = (TSList) malloc( sizeof(TSListData) );
  
  list->data = (Item*) malloc( LIST_LENGTH_INCREMENT * sizeof(Item) );
  for( size_t position = 0; position < LIST_LENGTH_INCREMENT; position++ )
    list->data[ position ].data = malloc( itemSize );
  
  list->length = LIST_LENGTH_INCREMENT;
  list->itemsCount = list->insertCount = 0;
  list->itemSize = itemSize;
  
  list->accessLock = TLock_Create();
  
  return list;
}

void TSL_Discard( TSList list )
{
  if( list != NULL )
  {
    for( size_t dataIndex = 0; dataIndex < list->itemsCount; dataIndex++ )
        free( list->data[ dataIndex ].data );
    free( list->data );
    
    TLock_Discard( list->accessLock );

    free( list );
    list = NULL;
  }
}

size_t TSL_GetItemsCount( TSList list )
{
  return list->length;
}

int ListCompare( const void* ref_item_1, const void* ref_item_2 )
{
  return ( ((Item*) ref_item_1)->key - ((Item*) ref_item_2)->key );
}

size_t TSL_Insert( TSList list, void* dataIn )
{
  TLock_Acquire( list->accessLock );
  
  if( list->itemsCount + 1 > list->length )
  {
    list->length += LIST_LENGTH_INCREMENT;
    list->data = (Item*) realloc( list->data, list->length * sizeof(Item) );
  }
  
  list->data[ list->itemsCount ].key = list->insertCount;
  
  list->data[ list->itemsCount ].data = (void*) malloc( list->itemSize );
  memcpy( list->data[ list->itemsCount ].data, dataIn, list->itemSize );
  
  qsort( (void*) list->data, list->length, list->itemSize, ListCompare );
  
  list->insertCount++;
  list->itemsCount++;
  
  TLock_Release( list->accessLock );
  
  return list->insertCount;
}

int TSL_GetIndexKey( TSList list, size_t index )
{
  if( list == NULL ) return -1;
  
  if( index > list->itemsCount ) return -1;
  
  return list->data[ index ].key;
}

bool TSL_Remove( TSList list, int key )
{
  TLock_Acquire( list->accessLock );
  
  if( list->itemsCount == 0 ) return false;
  
  Item comparisonItem = { key, NULL };
  
  Item* foundItem = (Item*) bsearch( (void*) &comparisonItem, (void*) list->data, list->length, list->itemSize, ListCompare );
  if( foundItem == NULL ) return false;
  
  free( foundItem->data );
  foundItem->key = 0xFFFFFFFF;
  
  qsort( (void*) list->data, list->length, list->itemSize, ListCompare );
  
  list->itemsCount--;
  
  if( list->itemsCount < list->length - LIST_LENGTH_INCREMENT )
  {
    list->length -= LIST_LENGTH_INCREMENT;
    list->data = (Item*) realloc( list->data, list->length * sizeof(Item) );
  }
  
  TLock_Release( list->accessLock );
  
  return true;
}

void* TSL_AcquireItem( TSList list, int key )
{
  Item comparisonItem = { key, NULL };
  
  TLock_Acquire( list->accessLock );
  
  Item* foundItem = (Item*) bsearch( (void*) &comparisonItem, (void*) list->data, list->length, list->itemSize, ListCompare );
  if( foundItem == NULL ) return NULL;
  
  return foundItem->data;
}

void TSL_ReleaseItem( TSList list )
{
  TLock_Release( list->accessLock );
}

bool TSL_GetItem( TSList list, int key, void* dataOut )
{
  void* foundData = TSL_AcquireItem( list, key );
  if( foundData == NULL ) return false;
  
  if( list->itemSize > sizeof(void*) ) foundData = &foundData;
    
  if( dataOut != NULL ) memcpy( dataOut, foundData, list->itemSize );
    
  TSL_ReleaseItem( list );
  
  return true;
}

bool TSL_SetItem( TSList list, int key, void* dataIn )
{
  void* foundData = TSL_AcquireItem( list, key );
  if( foundData == NULL ) return false;
  
  if( list->itemSize > sizeof(void*) ) foundData = &foundData;
  
  if( dataIn != NULL ) memcpy( foundData, dataIn, list->itemSize );
  
  TSL_ReleaseItem( list );
  
  return true;
}
