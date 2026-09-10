#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

/* Virtual Memory assignment
   Write my own methods to: 
   init()  initialize virtual memory,
   myMalloc()  allocate a memory block, and 
   myFree()  free memory.
   main() runs a few test cases and prints out
          the memory pool each time.
   @author: Tammy Hansen */

static int* memPool;
static int* p;

int SIZE = 256;

void markBlockLoc(int loc, int len, bool aFlag)
{
    int mark = len;   //mark to use if unallocated

    if (aFlag)  //allocation flag, true = allocated
    {
        mark += 1;       //allocated: make it an odd number
    }
    

    memPool[loc] = mark;         //mark start of block
    memPool[loc+len-1] = mark;   //mark end of block
}

void markBlockPtr(int* p, int len, bool aFlag)
{
    int mark = len;   //mark to use if unallocated

    if (aFlag)  //allocation flag, true = allocated
    {
        mark += 1;       //allocated: make it an odd number
    }
    

    p[0] = mark;         //mark start of block
    p[len-1] = mark;     //mark end of block
}

void init()
{
    //allocate memory for the memory pool
    memPool = (int *)malloc(SIZE * sizeof(int));
    if (memPool == NULL)
    {
        perror("malloc");
        exit(0);
    } 
    //initialize memPool to zero
    for (int i = 0; i < 256; i++)
    {
        memPool[i] = 0;
    }
    
    //mark begin & end of memory pool
    markBlockLoc(0, SIZE, false);
}



void addBlock(int loc, int len) { 
  int newSize = ((len + 1) >> 1) << 1;  // round up to even 
  int oldSize = memPool[loc] & -2;      // mask out low bit 
  
  newSize += 2;                         // add 2 to newSize for header & footer

  //mark the begin & end of allocated block
  markBlockLoc(loc, newSize, true);
  markBlockLoc(loc+newSize, oldSize-newSize, false);
}

int traverse(int len)
{
    int loc = 0;
    //traverse the memPool for a block that's just right
    while (loc < SIZE)
    {
        if (!(memPool[loc] & 1) &&         //not already allocated or
             (memPool[loc] > len + 2))     //not too small
             {
               return loc;                 //next available block
             }
        else 
        {
            loc += (memPool[loc] & -2);
        }
    }  

    //what if we reach the end of memPool and didn't find anything?
    if (loc == SIZE)
    {
        return -1;
    }
    else
    {
        return loc;
    }
}

int* myMalloc(int len)
{
    //if not a multiple of 4, get to next length in multiples of 4
    if (len % 4 != 0)
    {
        len = (len / 4 + 1) * 4;
    }

    int loc = traverse(len);
    if (loc == -1)
        {
            perror("no block available");
            exit(0);
        }
    else
    {
        //add a new allocated block in memPool
        addBlock(loc, len);
        return &memPool[loc+1];
    }
}

void myFree (int* p) { 
   
    //at beginning of block (location loc)
    int len = *(p-1) &-2;          //pull block length from block header
    
    /*initialize a new start location and size:
    if there's no next or prev free block,
    location determined by p pointer and block size = len */
    int* newStart = p-1;
    int newSize = len;


    //check next block:
    int* nextBlock = p + len -1;           
    int nextSize = *nextBlock & -2;

    /* if the flag bit is zero it's free:
        include the next block size into newSize */
    if ((*nextBlock &1) == 0)
    {
       newSize += nextSize;
    }
    
    //check previous block:
    int* prevBlock = p - 2;
    int prevSize = *prevBlock & -2;

    /* if the flag bit is zero it's free:
        include the previous block size into newSize,
        and adjust newStart to include previous block start */
    if (((*prevBlock &1) == 0) &&
         prevSize != 0)
    {
        newStart = prevBlock - prevSize + 1;
        newSize += prevSize;
    }
    markBlockPtr(newStart, newSize, false);     //use markBlock to change start to free
    
}

void print_memPool()
{
    //simple method to look at current memPool
    for (int i = 0; i < SIZE; i++)
    {
         if (i % 8 == 0)
            printf("\n");
        printf("%i ", memPool[i]);
    }
}
int main(void)
{
    //initialize empty memPool
    init();
    printf("\n\n memPool initialized:\n");
    print_memPool();

    //allocate 10 blocks
    int* p[10];
    for(int i = 0; i < 10; i++)
    {
        p[i] = myMalloc(10+i);
        printf("\n\n%i\n", i);
        print_memPool();
    }

    //free first block
    myFree(p[0]);
    printf("\n\n after free 0\n");
    print_memPool();

    //free third block
    myFree(p[2]);
    printf("\n\n after free 2\n");
    print_memPool();

    //free second block (and should coalesce)
    myFree(p[1]);
    printf("\n\n after free 1\n");
    print_memPool();

    //free the rest of it; 1st and last should return to 256
    for(int i = 3; i < 10; i++)
    {
        myFree(p[i]);
    printf("\n\n after free %i\n", i);
    print_memPool();
    }
}