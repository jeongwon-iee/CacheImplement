# Cache Implementation
Computer Architecture :: Cache Implement in C  
completed 12/8/2018

## Cache Specification
 - Cache size: 32 bytes  
 - Block size: 8 bytes  
    - For direct mapped cache  
    4 cache spaces  
    - For 2-way set associative cache  
    2 sets & 2 entries per set  
    – Fully associative cache  
    4 entries   
  - Memory: 128 words (128*4 bytes)  
    - Little endianness (Intel)  
  - Cache access time: 1 clock cycle  
  - Memory access time: 100 clock cycles 
  
## Procedure
   1) Invoke init_memory_content(); 
   2) Invoke init_cache_content(); 
   3) open “access_input.txt” file 
   4) Read each line 
   5) Try to access the cache to check whether the data is available in the cache; If so, access it directly from the cache 
   6) Otherwise, attempt to access the main memory and update the cache 
   7) Print the accessed data into “access_output.txt” 
   8) Increment global_timestamp by 1 
   9) Go to 4)

## Input & Output
  - Input:  
    Integer Byte address and Byte size (b:Byte, h:Halfword, w:Word)
  
  - Output:  
    (1) Accessed data at a specific address  
    (2) Hit ratio = # of hits / # of accesses  
    (3) Bandwidth = # of bytes / # of clock cycles  
