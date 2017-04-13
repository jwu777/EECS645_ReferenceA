
#ifndef cache_simulator_h
#define cache_simulator_h

/**
**/
#define CacheSizeExp 15
#define CacheSize (1 << CacheSizeExp)

#define AddressSizeBits 32

#define AddressSizeByte (AddressSizeBits >> 3)

/**
  Cache Size
**/
#define CacheAssociativityExp 2
#define CacheAssociativity (1 << CacheAssociativityExp)

#define BlockSizeExp 6
#define BlockSize (1 << BlockSizeExp)
#define BlockSizeMask (BlockSize - 1)

#define LineSizeExp (CacheSizeExp - (CacheAssociativityExp + BlockSizeExp))
#define LineSize (1 << LineSizeExp)
#define LineSizeMask (LineSize - 1)

#define TagExp (AddressSizeBits - (BlockSizeExp + LineSizeExp))
#define TagSize (1 << TagExp)
#define TagSizeMask (TagSize - 1)

int cacheHit = 0;
int cacheMiss = 0;

/**
	Make a cache, with every line being invalid
**/
/** typedef added "clean up" code and reduce repetition of "struct" **/
typedef struct CacheLine {
  bool valid;
  bool full;
  uint32_t tag;
} CacheLine; /** struct variable **/

CacheLine cache[LineSize][CacheAssociativity] = {};

void PrintParameters();

/** Replacement Algorithm **/
void updateCache(uint32_t address);

void ReadFromTraceFile();

#endif
