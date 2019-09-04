#pragma once
/*
* cache_impl.h
*
* 20493-02 Computer Architecture
* Term Project on Implentation of Cache Mechanism
*
* Skeleton Code Prepared by Prof. HyungJune Lee
* Nov 13, 2017
*
*/

/* DO NOT CHANGE THE FOLLOWING DEFINITIONS EXCEPT 'DEFAULT_CACHE_ASSOC */

#ifndef _CACHE_IMPL_H_
#define _CACHE_IMPL_H_

#define WORD_SIZE_BYTE                   4	 //the number of bytes in one word 
#define DEFAULT_CACHE_SIZE_BYTE          32	 //cache size in byte
#define DEFAULT_CACHE_BLOCK_SIZE_BYTE    8	 //the number of bytes in one cache block
#define DEFAULT_CACHE_ASSOC              4   /* This can be changed to 1(for direct mapped cache) or 4(for fully assoc cache) */
#define DEFAULT_MEMORY_SIZE_WORD         128 //memory size in word 
#define CACHE_ACCESS_CYCLE               1	 //it took 1 cycle to accessing cache
#define MEMORY_ACCESS_CYCLE              100 //it took 100 cycles accessing memory
#define CACHE_SET_SIZE                   ((DEFAULT_CACHE_SIZE_BYTE)/(DEFAULT_CACHE_BLOCK_SIZE_BYTE*DEFAULT_CACHE_ASSOC))

/* Function Prototypes */
void init_memory_content(); //initialize memory
void init_cache_content(); //initialize cache
void print_cache_entries(); //prints out all entries in cache
int check_cache_data_hit(void* addr, char type); //find index of cache entry to copy data from memory
int access_memory(void *addr, char type); //access memory when cache miss occurs, and copy data into cache


/* Cache Entry Structure */
typedef struct cache_entry {
	int valid; //for indicating this entry has data
	int tag; //for distinguising data within a set
	int timestamp; //inform when the cache entry was accessed
	char data[DEFAULT_CACHE_BLOCK_SIZE_BYTE]; //data field
} cache_entry_t;


#endif