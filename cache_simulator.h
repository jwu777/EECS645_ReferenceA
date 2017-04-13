
#ifndef cache_simulator_h
#define cache_simulator_h

#define CacheSize_Exp       15
#define CacheSize_Nbr       (1 << CacheSize_Exp)

#define AddressSize_Exp     32

#define Associativity_Exp   2
#define Associativity       (1 << Associativity_Exp)

#define BlockSize_Exp       6
#define BlockSize           (1 << BlockSize_Exp)

#define Lines_Exp           ((CacheSize_Exp) - (Associativity_Exp + BlockSize_Exp))
#define LineNbr             (1 << Lines_Exp)
#define Lines_Mask          (LineNbr - 1)

#define Tag_Exp             (AddressSize_Exp - BlockSize_Exp - Lines_Exp)
#define TagNbr              (1 << Tag_Exp)
#define Tag_Mask            (TagNbr - 1)

/**
	Make a cache, with every line being invalid
**/
/** typedef added "clean up" code and reduce repetition of "struct" **/
typedef struct CacheLine
{
	int valid;
	int tag;
	int recent;

} CacheLine; /** struct variable **/

CacheLine cache[LineNbr][Associativity];

int hits = 0, misses = 0;
float hitRatio = 0;

#endif
