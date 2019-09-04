/*
* cache.c
*
* 20493-02 Computer Architecture
* Term Project on Implentation of Cache Mechanism
*
* Skeleton Code Prepared by Prof. HyungJune Lee
* Nov 13, 2017
*
*/


#include <stdio.h>
#include <string.h>
#include "cache_impl.h"

extern int num_cache_hits; //the number of occurrences of cache hit
extern int num_cache_misses; //the number of occurrences of cache miss

extern int num_bytes; //total number of bytes of data we are looking for
extern int num_access_cycles; //total number of times to access cache or memory to find data

extern int global_timestamp; //current time of an event that is recorded by a computer

cache_entry_t cache_array[CACHE_SET_SIZE][DEFAULT_CACHE_ASSOC];
int memory_array[DEFAULT_MEMORY_SIZE_WORD];


/* DO NOT CHANGE THE FOLLOWING FUNCTION */
void init_memory_content() {
	unsigned char sample_upward[16] = { 0x001, 0x012, 0x023, 0x034, 0x045, 0x056, 0x067, 0x078, 0x089, 0x09a, 0x0ab, 0x0bc, 0x0cd, 0x0de, 0x0ef };
	unsigned char sample_downward[16] = { 0x0fe, 0x0ed, 0x0dc, 0x0cb, 0x0ba, 0x0a9, 0x098, 0x087, 0x076, 0x065, 0x054, 0x043, 0x032, 0x021, 0x010 };
	int index, i = 0, j = 1, gap = 1;

	for (index = 0; index < DEFAULT_MEMORY_SIZE_WORD; index++) {
		memory_array[index] = (sample_upward[i] << 24) | (sample_upward[j] << 16) | (sample_downward[i] << 8) | (sample_downward[j]);
		if (++i >= 16)
			i = 0;
		if (++j >= 16)
			j = 0;

		if (i == 0 && j == i + gap)
			j = i + (++gap);

		printf("mem[%d] = %#x\n", index, memory_array[index]);
	}
}

/* DO NOT CHANGE THE FOLLOWING FUNCTION */
void init_cache_content() {
	int i, j;

	for (i = 0; i < CACHE_SET_SIZE; i++) {
		for (j = 0; j < DEFAULT_CACHE_ASSOC; j++) {
			cache_entry_t *pEntry = &cache_array[i][j];
			pEntry->valid = 0;
			pEntry->tag = -1;
			pEntry->timestamp = 0;
		}
	}
}

/* DO NOT CHANGE THE FOLLOWING FUNCTION */
/* This function is a utility function to print all the cache entries. It will be useful for your debugging */
void print_cache_entries() {
	int i, j, k;

	for (i = 0; i < CACHE_SET_SIZE; i++) {
		printf("[Set %d] ", i);
		for (j = 0; j < DEFAULT_CACHE_ASSOC; j++) {
			cache_entry_t *pEntry = &cache_array[i][j];
			printf("V: %d Tag: %#x Time: %d Data: ", pEntry->valid, pEntry->tag, pEntry->timestamp);
			for (k = 0; k < DEFAULT_CACHE_BLOCK_SIZE_BYTE; k++) {
				printf("%#x(%d) ", pEntry->data[k], k);
			}
			printf("\t");
		}
		printf("\n");
	}
}

/* This function returns the value of the data if it is in the cache, and -1 if it is not */
int check_cache_data_hit(void *addr, char type) {

	int address = *(int*)addr; //since void *addr stores the address of the memory, convert the generic pointer to int pointer and use the dereferencing(*) operator to access the value
	int block_addr = address / DEFAULT_CACHE_BLOCK_SIZE_BYTE; //divide address which is in bytes by the number of bytes per block to obtain block address
	int byte_offset = address % DEFAULT_CACHE_BLOCK_SIZE_BYTE; //the remainder of address divided by the number of bytes per block is the offset
	int tag = block_addr / CACHE_SET_SIZE; //tag is the part of the block address that excludes the index bit, so divide the block address by the number of sets
	int cache_index = block_addr % CACHE_SET_SIZE; //index which describes which cache set the data has been put in is the remainder of block address divided by the number of sets

	int value_returned = 0x0; //to store the value of data if it exists in cache
	int temp_cache_data[DEFAULT_CACHE_BLOCK_SIZE_BYTE] = { 0 }; //temporary storage for copied cache data
	int shift = 0; //to shift left the one byte data to combine the data in different array index
	int data_length = 0; //data's byte size(ex. byte=1)

	num_access_cycles += CACHE_ACCESS_CYCLE; //since cache is accessed, so add the number of cache access cycles to total number of access cylcles

	for (int i = 0; i < DEFAULT_CACHE_ASSOC; i++) {
		cache_entry_t * pEntry = &cache_array[cache_index][i];
		if (pEntry->valid == 1 && pEntry->tag == tag) {
			/* when cache entry's valid is 1, and the entry's tag is equal to given data's tag, cache hit occurs */
			num_cache_hits++; //increase the number of hits
			cache_array[cache_index][i].timestamp = global_timestamp++; //if cache entry is accessed, update the timestamp of that entry to the current timestamp
			
			/* copy the data in the cache to "temp_cache_data" due to prevent modification of original cache data */
			for (int i = 0; i < DEFAULT_CACHE_BLOCK_SIZE_BYTE; i++) {
				temp_cache_data[i] = pEntry->data[i];
			}
			/* the byte size of the data will be stored in data_length */
			switch (type) { //b, h, or w would be stored in the argument 'type' in this function
			case 'b': //if the data size is byte,
				data_length = 1; //1 is assigned in data_length
				break;
			case 'h': //if the data size is half word,
				data_length = 2; //2 is assigned in data_length
				break;
			case 'w': //if the data size is word,
				data_length = 4; //4 is assigned in data_length
				break;
			}
			/* cache splits one block to 8 bytes and stores them into an array*/
			/* To return data, the splited bytes should be concatinated*/
			/* read one byte from the "temp_cache_data" as the size of the data type*/
			/* Each iterations, increase "shift" by 8*/
			for (int i = byte_offset; i < (byte_offset + data_length); i++) {
				value_returned += (unsigned char)temp_cache_data[i] << shift; //before add the byte data, shift left by the value of "shift"
				shift += 8; //increase "shift" by 8
			}

			num_bytes += data_length; //add the size of this data to "num_bytes"
			return value_returned; //return the data accessed in cache
		}
	}
	
	num_cache_misses++; //in case of miss, increase the number of miss
	return -1; //if the data we're looking for is not exists in cache (cache miss occurs), return -1
}

/* This function finds empty entry of cache to copy the data from memory and returns the index of that entry when cache miss occurs,
	and if there is no free space, returns an index of the least recently accessed cache entry */
int find_entry_index_in_set(int cache_index) {

	int entry_index = 0; //cache location to store data copied from memory
	int min_timestamp; //to save the minimum timestamp for finding cache entry to store new data when the cache is full
	int temp_entry_index = 0; //to store timestamp when performing iterative comparative operation

	/* Check if there exists any empty cache space by checking 'valid' */
	for (entry_index = 0; entry_index < DEFAULT_CACHE_ASSOC; entry_index++) {
		cache_entry_t *pEntry = &cache_array[cache_index][entry_index]; //create a head pointer to access each element in the cache
		if (pEntry->valid == 0) { //if the cache entry's valid is 0, it means it is an empty entry
			return entry_index; //so return the index of that cache entry
		}
	}

	/* Otherwise, search over all entries to find the least recently used entry by checking 'timestamp'*/
	/* The least recently accessed cache entry will have the minimum value of timestamp, 
		so compare the timestamp of each element of cache and find the minimum value */
	min_timestamp = cache_array[cache_index][0].timestamp; //initialize min_timestamp with the first entry's timestamp
	for (entry_index = 1; entry_index < DEFAULT_CACHE_ASSOC; entry_index++) { //access each entry in the cache increasing value entry_index
		cache_entry_t *pEntry = &cache_array[cache_index][entry_index]; //create a head pointer to access each element in the cache
		if (pEntry->timestamp < min_timestamp) { //if the entry's timestamp is smaller than min_timestamp
			min_timestamp = pEntry->timestamp; //update the min_timestamp value to that entry's timestamp
			temp_entry_index = entry_index; //store the index value of that entry temporarily in temp_entry_index
		}
	}
	entry_index = temp_entry_index; //temp_entry_index will have the minimum value of the timestamp, so move it back to entry_index
	return entry_index; //return the index of entry which has minimum timestamp

}

/* If the data we're looking for does not exists in the cache, this function accesses the memory, copy that value, find an entry in cache then copy it */
int access_memory(void *addr, char type) {

	/* Fetch the data from the main memory and copy them to the cache */
	/* void *addr: addr is byte address, whereas your main memory address is word address due to 'int memory_array[]' */

	char block[DEFAULT_CACHE_BLOCK_SIZE_BYTE] = { 0 }; //temporary storage for copied block data
	int temp_memory_array[20] = { 0 }; //temporary storage for copied memory data
	int temp_memory_index; //index of temp_memory_array
	int value_returned = 0x0; //to store the value of data
	int address = *(int*)addr; //since void *addr stores the address of the memory, convert the generic pointer to int pointer and use the dereferencing(*) operator to access the value
	int block_addr = address / DEFAULT_CACHE_BLOCK_SIZE_BYTE; //divide address which is in bytes by the number of bytes per block to obtain block address
	int byte_offset = address % DEFAULT_CACHE_BLOCK_SIZE_BYTE; //the remainder of address divided by the number of bytes per block is the offset
	int tag = block_addr / CACHE_SET_SIZE; //tag is the part of the block address that excludes the index bit, so divide the block address by the number of sets
	int cache_index = block_addr % CACHE_SET_SIZE; //index which describes which cache set the data has been put in is the remainder of block address divided by the number of sets
	int entry_index; //saves the return value of function find_entry_index_in_set

	int word_index = block_addr * DEFAULT_CACHE_BLOCK_SIZE_BYTE / WORD_SIZE_BYTE; //points to the word unit within a block of memory
	int shift = 0; //to shift left the one byte data to combine the data in different array index
	int data_length = 0; //data's byte size(ex. byte=1)

	num_access_cycles += MEMORY_ACCESS_CYCLE; //since memory is accessed, so add the number of memory access cycles to total number of access cylcles

	/* store 1 word to (DEFAULT_CACHE_BLOCK_SIZE_BYTE/WORD_SIZE_BYTE) number of temporary memory storage */
	for (int i = 0; i < (DEFAULT_CACHE_BLOCK_SIZE_BYTE / WORD_SIZE_BYTE); i++) {
		temp_memory_array[i] = memory_array[word_index + i]; //copy (word_index+i)th word in the memory to ith temporary memory storage
	}

	/* concatenate words of memory and store them into one "block" array */
	/* this memory stores data as "little endian" way, take one byte from rigth to left of the memory at each iterations */
	for (int i = 0; i < DEFAULT_CACHE_BLOCK_SIZE_BYTE; i += WORD_SIZE_BYTE) { //"i" is the start index of new word of "block", which is multiple of WORD_SIZE_BYTE
		temp_memory_index = i / WORD_SIZE_BYTE; //index of temp_memory_index is from 0 to (the number of word in the block)-1
		for (int j = i; j < (i + WORD_SIZE_BYTE); j++) { //store one byte from temp_memory_array to "block" array in word
			block[j] = (char)(temp_memory_array[temp_memory_index] % 0x100); //get one byte data from least significant byte of temporary memory data storage
			temp_memory_array[temp_memory_index] >>= 8; //remove least significant byte of temporary memory storage
		}
	}
	/* You need to invoke find_entry_index_in_set() for copying to the cache */
	entry_index = find_entry_index_in_set(cache_index); //the index of entry of the cache which is empty or has minimum timestamp will be stored

	/* move the data in "block" array to "cache_arry" by one byte */
	for (int i = 0; i < DEFAULT_CACHE_BLOCK_SIZE_BYTE; i++) { //increase index of cache data array and block array
		cache_array[cache_index][entry_index].data[i] = block[i]; //move one byte from block to cache
	}

	cache_array[cache_index][entry_index].timestamp = global_timestamp++; //cache entry is accessed, so update the timestamp of that entry to the current timestamp
	cache_array[cache_index][entry_index].valid = 1; //the value is stored in the cache, update the valid to 1
	cache_array[cache_index][entry_index].tag = tag; //the tag of the entry is also has to be updated with the tag value of the copied data 

	/* the byte size of the data will be stored in data_length */
	switch (type) { //b, h, or w would be stored in the argument 'type' in this function
	case 'b': //if the data size is byte,
		data_length = 1; //1 is assigned in data_length
		break;
	case 'h': //if the data size is half word,
		data_length = 2; //2 is assigned in data_length
		break;
	case 'w': //if the data size is word,
		data_length = 4; //4 is assigned in data_length
		break;
	}

	/* this "block" splits one block to 8 bytes and stores them into an array */
	/* To return data, the splited bytes should be concatinated */
	/* read one byte from the "block" as the size of the data type */
	/* Each iterations, increase "shift" by 8 */
	for (int i = byte_offset; i < (byte_offset + data_length); i++) {
		value_returned += (unsigned char)block[i] << shift; //before add the byte data, shift left by the value of "shift"
		shift += 8;//increase "shift" by 8
	}

	num_bytes += data_length; //add the size of this data to "num_bytes"
	/* Return the accessed data with a suitable type */
	return value_returned;
}