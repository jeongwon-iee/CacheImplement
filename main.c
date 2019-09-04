/*
* main.c
*
* 20493-02 Computer Architecture
* Term Project on Implentation of Cache Mechanism
*
* Skeleton Code Prepared by Prof. HyungJune Lee
* Nov 13, 2017
*
*/

#include <stdio.h>
#include "cache_impl.h"

int num_cache_hits = 0; //the number of occurrences of cache hit
int num_cache_misses = 0; //the number of occurrences of cache miss

int num_bytes = 0; //total number of bytes of data we are looking for
int num_access_cycles = 0;//total number of times to access cache and memory to find data

int global_timestamp = 0; //current time of an event that is recorded by a computer, 
						  //needed when updating the cache entry's timestamp when cache entry has been accessed

/* Retrieve data that the address indicates from cache or main memory */
int retrieve_data(void *addr, char data_type) {
	int value_returned = -1; /* accessed data */

	/* Invoke check_cache_data_hit() */
	value_returned = check_cache_data_hit(addr, data_type); //stores the value of the data if it is in the cache, and -1 if it is not

	if (value_returned == -1) { //In case of the cache miss event,
		value_returned = access_memory(addr, data_type); //access the main memory by invoking access_memory()
	}

	/* Return the data to main() fuction*/
	return value_returned;
}

int main(void) {
	FILE *ifp = NULL, *ofp = NULL;  //make a File pointers for input file and output file and set them NULL.
	unsigned long int access_addr; /* byte address (located at 1st column) in "access_input.txt" */
	char access_type; /* 'b'(byte), 'h'(halfword), or 'w'(word) (located at 2nd column) in "access_input.txt" */
	int accessed_data; /* This is the data that you want to retrieve first from cache, and then from memory */

	unsigned long int *address_ptr = &access_addr; //pointer for "access_addr" variable which has data's adrress
	int flag = 0; // used to check whether inputed data was address or type 
	char ch; //temporary variable for inputed charactor
	
	init_memory_content(); //initialize memory
	init_cache_content(); //initialize cache

	/* open input file and output file, then pointers refer the files */
	/* if the files don't be opened, notify the error then terminate this program*/
	ifp = fopen("access_input.txt", "r");
	if (ifp == NULL) {
		printf("Can't open input file\n");
		return -1;
	}
	ofp = fopen("access_output.txt", "w");
	if (ofp == NULL) {
		printf("Can't open output file\n");
		fclose(ifp);
		return -1;
	}

	fprintf(ofp, "[Accessed Data]\n"); //print this line before print the results
									   /* Until end of reading data in the input file, repeat the following expressions*/
	while (1) {
		if (flag == 0) { //if flag is 0, input data may be address
			fscanf(ifp, "%d", &access_addr); //accept address value from input file
			flag = 1; //change the value of flag to 1
		}
		else {
			ch = getc(ifp); //get one charcter from the input file
			if (ch == EOF) break; //if the charcter is EOF(end of file), stop reading data
			if (ch == 'b' || ch == 'h' || ch == 'w') { //if the entered character is 'b' or 'h' or 'w'
				access_type = ch; // it means, ch is "type", so store the value into "access_type"
				flag = 0; //change the value of flag to 0
				accessed_data = retrieve_data(address_ptr, access_type); //call retrieve data() and accept "accessed_data"
				fprintf(ofp, "%6d%6c%#15x\n", access_addr, access_type, accessed_data); //print access address, access data type and accessed data into the output file
			}
		}
	}
	fprintf(ofp, "--------------------------------------------\n"); //below this line, hit ratio and bandwidth value of these datas will be printed

	//select the type of cache by using "DEFAULT_CACHE_ASSOC" value, then print this
	switch (DEFAULT_CACHE_ASSOC) {
	case 1: //if DEFAULT_CACHE_ASSOC is 1, it means it is Direct mapped cache,
		fprintf(ofp, "[Direct mapped cache performance]\n"); //so print like this
		break;
	case 2: //if DEFAULT_CACHE_ASSOC is 2, it means it is 2-way set associative cache,
		fprintf(ofp, "[2-way set associative cache performance]\n"); //so print like this
		break;
	case 4: //if DEFAULT_CACHE_ASSOC is 4, it means it is fully associative cache,
		fprintf(ofp, "[Fully associative cache performance]\n"); //so print like this
		break;
	}

	/* calculate hit ratio and bandwidth of these input datas, then print them on the output file */
	fprintf(ofp, "Hit ratio = %.2lf (%d/%d)\n", (double)num_cache_hits / (num_cache_misses + num_cache_hits), num_cache_hits, (num_cache_misses + num_cache_hits)); //prints the hit ratio
	fprintf(ofp, "Bandwidth = %.2lf (%d/%d)\n", (double)num_bytes / num_access_cycles, num_bytes, num_access_cycles); //prints the bandwidth

	/*close the files*/
	fclose(ifp);
	fclose(ofp);

	print_cache_entries(); //print all entries in the cache
	return 0;
}