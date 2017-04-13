
/** Cache Simulator written in C **/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "cache_simulator.h"


void createCache()
{
  int i;
  int j;

   for(i = 0; i < LineNbr; i++)
   {
     for(j = 0; j < Associativity; j++)
     {
       cache[i][j].valid = 0;
       cache[i][j].tag = 0;
       cache[i][j].recent = 0;
     }
   }
}

void printCacheData()
{
   printf("CacheSize_Exp: %d\n", CacheSize_Exp);
   printf("CacheSize_Nbr: %d\n", CacheSize_Nbr);
   printf("AddressSize_Exp: %d\n", AddressSize_Exp);
   printf("Associativity: %d\n", Associativity);
   printf("BlockSize_Exp: %d\n", BlockSize_Exp);
   printf("BlockSize: %d\n", BlockSize);
   printf("Lines_Exp: %d\n", Lines_Exp);
   printf("LineNbr: %d\n", LineNbr);
   printf("Lines_Mask: %d\n", Lines_Mask);
   printf("Tag_Exp: %d\n", Tag_Exp);
   printf("TagNbr: %d\n", TagNbr);
   printf("Tag_Mask: %d\n", Tag_Mask);
}

long decimalToBinary(unsigned char character[])
{
   long test = (unsigned char)(character);
   long remainder;
   long binary = 0;
   long j = 1;

       while(test != 0){
         remainder = test%2;
         test = test/2;
         binary = binary + (remainder * j);
         j = j*10;
       }

       return binary;
}

void incrementTime()
{
  int i;
  int j;

  for(i = 0; i < LineNbr; i++)
   {
     for(j = 0; j < Associativity; j++)
     {
       cache[i][j].recent++;
     }
   }

}

void LRU(long binary, int cacheLine, int blockIndex)
{
 int longestI = 0;
 int longestJ = 0;
 int longestTime = 0;

 int i;
 int j;

   for(i = 0; i < LineNbr; i++)
   {
     for(j = 0; j < Associativity; j++)
     {
       if(cache[i][j].recent >= longestTime)
       {
         longestI = i;
         longestJ = j;
       }
     }
   }

   cache[longestI][longestJ].tag = binary;
   cache[longestI][longestJ].valid = 1;

}


int main(void)
{
   /**Declare variables**/

   FILE *traceFile;
   char* filePath = "";
   int indexOption = 0;
   unsigned char address[AddressSize_Exp];

   /**Menu options for easy usability**/

   printf("Welcome to Cache Simulator!\n\n");

   printCacheData();

   printf("\nPlease choose a binary file:\n");
   printf("1) First Index\n");
   printf("2) Last Index\n");
   printf("3) Random Index\n");
   scanf("%d", &indexOption);

   /**Decide, based on user input, which file to open**/

   switch(indexOption){
       case 1:
           filePath = "AddressTrace_FirstIndex.bin";
           break;
       case 2:
           filePath = "AddressTrace_LastIndex.bin";
           break;
       case 3:
           filePath = "AddressTrace_RandomIndex.bin";
           break;

   }

   /**Open file**/
   if((traceFile = fopen(filePath, "r")) == NULL)
   {
     printf("Error in opening file. Exiting...\n");
   }

   /**Initialize global cache**/

   createCache();

   /**Reading the cache while scanning the file**/


   int blockIndex = 0;
   int foundVal = 0;
   long binary = 0;
   long cacheLine = 0;
   long cacheTag = 0;

   while((fread(&address, 4, 1, traceFile) != 0))
   {
       foundVal = 0;
       blockIndex = 0;
       binary = decimalToBinary(address);
       cacheLine = (binary >> BlockSize_Exp) & Lines_Mask;
       cacheTag = ((binary >> (BlockSize_Exp + Lines_Exp)) & Tag_Mask);

       for(;blockIndex < Associativity; blockIndex++)
       {
         if(cache[cacheLine][blockIndex].tag == cacheTag)
         {
           hits++;
           incrementTime();
           cache[cacheLine][blockIndex].recent = 0;
           foundVal = 1;
           break;
         }
       }

       if(foundVal == 0)
       {
         misses++;
         incrementTime();
         LRU(cacheTag, cacheLine, blockIndex);
       }
   }

   /**Information about cache metrics**/
   hitRatio = ((hits/(misses+hits))*100.0);
   printf("Hits: %d\n", hits);
   printf("Misses: %d\n", misses);
   printf("Hit Ratio: %.02f%%\n", hitRatio);

   return 0;

}
