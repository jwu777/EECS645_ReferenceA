/**
**/

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>
#include "cache_simulator.h"


FILE* traceFile;

void PrintParameters() {
  printf("===============Cache Parameters===============\n");
  printf("Cache Size Exp: %d\n", CacheSizeExp);
  printf("Cache Size: %d\n", CacheSize);
  printf("Address Size Bits: %d\n", AddressSizeBits);
  printf("Address Size Bytes: %d\n", AddressSizeByte);
  printf("Block Size Exp: %d\n", BlockSizeExp);
  printf("Block Size: %d\n", BlockSize);
  printf("Block Size Mask: %d\n", BlockSizeMask);
  printf("Line Size Exp: %d\n", LineSizeExp);
  printf("Line Size: %d\n", LineSize);
  printf("Line Size Mask: %d\n", LineSizeMask);
  printf("Tag Exp: %d\n", TagExp);
  printf("Tag Size: %d\n", TagSize);
  printf("Tag Size Mask: %d\n", TagSizeMask);
  printf("==============================================\n\n");
}

/**
 Function used to replace a block in the specified line of the
 cache if there was no hit. The updateCache algorithm used is FIFO.
**/
void updateCache(uint32_t address) {
  int BlockColumn = 0;
  bool replaced = false;
  bool hasEmpty = false;
  uint32_t Line = (address >> BlockSizeExp) & LineSizeMask;
  uint32_t Tag = ((address >> (BlockSizeExp + LineSizeExp)) & TagSizeMask);

  /**
   Check to see if there are any empty blocks in the line
  **/
  while(BlockColumn < CacheAssociativity) {

    /**
     If an empty block is found set flag to indicate this
    **/
    if(!cache[Line][BlockColumn].full){
      hasEmpty = true;
    }
    ++BlockColumn;
  }

  BlockColumn = 0;

  /**
    Loop to find which block needs to replace
  **/
  for(;BlockColumn < CacheAssociativity; ++BlockColumn) {

    /**
      If the line has an empty block replace the first empty block
    **/
    if(hasEmpty){

      /**
        Found the empty block
      **/
      if(!cache[Line][BlockColumn].full){

        /**
          Fill the empty block with the tag
        **/
        cache[Line][BlockColumn].tag = Tag;

        /**
          Set it to valid and full
        **/
        cache[Line][BlockColumn].valid = true;
        cache[Line][BlockColumn].full = true;
        replaced = true;
        break;
      }
      else {

        /**
          Set the block to invalid
        **/
        cache[Line][BlockColumn].valid = false;
      }
    }

    /**
      If all blocks in the line are full
    **/
    else{

      /**
        If the block is valid
      **/
      if(cache[Line][BlockColumn].valid) {
        if(BlockColumn == (CacheAssociativity-1)){

          /**
            Set the tag to the address
          **/
          cache[Line][0].tag = Tag;

          /**
            Set it to valid
          **/
          cache[Line][0].valid = true;
          cache[Line][BlockColumn].valid = false;
          replaced = true;
          break;
        }
        else{

          /**
            Set the tag to the address
          **/

          cache[Line][BlockColumn+1].tag = Tag;
          /**
            Set it to valid
          **/
          cache[Line][BlockColumn+1].valid = true;
          cache[Line][BlockColumn].valid = false;
          replaced = true;
          break;
        }
      }
      else {

        /**
          Set the block to invalid
        **/
        cache[Line][BlockColumn].valid = false;
      }
    }
  }

  /**
    If we never replaced (i.e. a line full of valids) replace the first one
  **/
  if(!replaced) {
    cache[Line][0].tag = Tag;
    cache[Line][0].valid = true;
  }

  /**
    If we did replace, make sure to invalidate all other blocks
  **/
  else if(replaced) {

    /**
      Don't invalidate the one we just replaced
    **/
    ++BlockColumn;
    while(BlockColumn < CacheAssociativity) {
      cache[Line][BlockColumn].valid = false;
      ++BlockColumn;
    }
  }
}

/**
 Function for reading an address from the given trace file and
 check if the cache returns a hit or miss for the address
**/
void ReadFromTraceFile() {

  uint32_t address;

  /**
    Read in each address from the file 32-bits each
  **/
  while (fread(&address, AddressSizeByte, 1, traceFile) != 0) {

    uint32_t Line = (address >> BlockSizeExp) & LineSizeMask;
    uint32_t Tag = ((address >> (BlockSizeExp + LineSizeExp)) & TagSizeMask);

    /**
      Used for associativity
    **/
    int BlockColumn = 0;
    bool Hit = false;

    /**
      Loop over the columns
    **/
    for(;BlockColumn < CacheAssociativity; ++BlockColumn) {
      /**
      If the cache[Line][Column]'s tag is equal to read in tag
      **/
      if(cache[Line][BlockColumn].tag == Tag) {

        /**
        We get a hit so increment the hit value and move on
        **/
        ++cacheHit;
        Hit = true;
        break;
      }
    }
    if(Hit == false) {
      /**
       Otherwise we got a miss
      **/

      ++cacheMiss;

      /**
       Call updateCache method
      **/
      updateCache(address);
    }

  }

  /**
   Print out the results
  **/
  printf("\n===============Cache Results===============\n");
  printf("Cache hit: %d\n", cacheHit);
  printf("Cache miss: %d\n", cacheMiss);
  printf("Cache hit ratio: %.02f%%\n", (float)(100.0 * cacheHit/(cacheMiss + cacheHit)));
  printf("===========================================\n\n");
}



void main(int argc, char *argv[]) {
  if(argc != 2){
    printf("Error: No filename entered\n");
  }
  else{
    traceFile = fopen(argv[1], "r");

    /**
     Print cache parameters
    **/
    PrintParameters();

    /**
     Read in the trace file
    **/
    ReadFromTraceFile();

    printf("%d/n",cache[2][5].tag);
    fclose(traceFile);
  }
}
