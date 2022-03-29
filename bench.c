/* Author: Zeyi Yan, Xiaoyu Zhou, Armando Jimenez
   UWNetID: zy75, xz081302, armanj08
   Course: CSE374
   Homework 6 bench.c
   date: 03/14/2022
   memory.c implements a bench test to test getmem and 
   freemem functions. It will run sepcific percent of
   getmem and limit memory size to a given range. During
   the test, it wll print out the feedbacks of current
   memory condition to the user
*/

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include "mem.h"

/* print_stats is a helper function that prints performance
   information about the memory system.  Requires a clock_t start
   time, and prints elapsed time plus results from get_mem_stats()
*/
void print_stats(clock_t start);

/* fill_mem is a helper function that takes in an address and the
   size of the memory allocation at that address.  It should fill
   the first 16 bytes (or up to size) with hexedecimal values.*/
void fill_mem(void* ptr, uintptr_t size);

/* Synopsis:   bench (main)
   [ntrials] (10000) getmem + freemem calls
   [pctget] (50) % of calls that are get mem
   [pctlarge] (10) % of calls requesting more memory than lower limit
   [small_limit] (200) largest size in bytes of small block
   [large_limit] (20000) largest size in byes of large block
   [random_seed] (time) initial seed for randn
*/
int main(int argc, char** argv ) {
  // Initialize the parameters
  int NTRIALS;
  int PCTGET;
  int PCTLARGE;
  int SMALL_L;
  int LARGE_L;

  (argc > 1) ? (NTRIALS = atoi(argv[1])) : (NTRIALS = 10000);
  (argc > 2) ? (PCTGET = atoi(argv[2])) : (PCTGET = 50);
  (argc > 3) ? (PCTLARGE = atoi(argv[3])) : (PCTLARGE = 10);
  (argc > 4) ? (SMALL_L = atoi(argv[4])) : (SMALL_L = 200);
  (argc > 5) ? (LARGE_L = atoi(argv[5])) : (LARGE_L = 20000);

  // initialize random number gen.
  (argc > 6) ? srand(atoi(argv[6])) : srand(time(NULL));

  printf("Running bench for %d trials, %d%% getmem calls.\n", NTRIALS, PCTGET);

  void* blocks[NTRIALS];  // upperbound block storage
  int ntrials = 0, nblocks = 0;  // nblock keeps track of blocks number
  clock_t start = clock();
  int tenth = 0;
  uintptr_t size;
  // perform NTRIALS mem operations
  for (ntrials = 0; ntrials <  NTRIALS; ntrials ++) {
    // there is probability pctlarge to get memory
    if (rand() % 100 < PCTGET) {
      // each time a new block is allocated, it should be a large block with
      // probability pctlarge
      if (rand() % 100 < PCTGET) {
        size = (uintptr_t) rand() % (LARGE_L - SMALL_L + 1) + SMALL_L;
      } else {
        size = (uintptr_t) rand() % (SMALL_L - 1 + 1) + 1;
      }
      void* ptr = getmem(size);
      if (ptr != NULL) {
        fill_mem(ptr, size);
      }
      blocks[nblocks] = ptr;
      nblocks++;
    } else {  // there is (100 - pctlarhe) probability to free memory
      // only free a block when there are previously allocated blocks
      // otherwise, do nothing
      if (nblocks) {
        int free_block_num = rand() % nblocks;
        freemem(blocks[free_block_num]);  // free random block
        blocks[free_block_num] = blocks[nblocks - 1];
        nblocks--;
      }
    }

    if ((ntrials - tenth) == NTRIALS/10) {
      print_stats(start);
      tenth = ntrials;
    }
  }

  return EXIT_SUCCESS;
}

/* Define helper functions here. */

// Print stats of benchmarking.
void print_stats(clock_t start) {
  uintptr_t total_size;
  uintptr_t total_free;
  uintptr_t n_free_blocks;
  get_mem_stats(&total_size, &total_free, &n_free_blocks);
  printf("Total CPU time used: %f s\n", (double)(clock() - start));
  printf("Total amount of storage acquired from underlying system: %lu\n",
         total_size);
  printf("Total number of blocks on the tree storage list: %lu\n",
         total_free);
  printf("Average number of bytes in the free storage blocks: %lu\n",
         n_free_blocks);
  printf("\n");
}

// Store the value 0xFE in each of the first 16 bytes of the
// allocated block starting at the pointer address returned by getmem
void fill_mem(void* ptr, uintptr_t size) {
  char initial[16];
  char* ptr_initial = (char*) ptr;
  for (int i =0; i< 16; i++) {
    initial[i] = 0xFE;
  }
  if (ptr != NULL) {
    int initial_size;
    if (size < 16) {
      initial_size = size;
    } else {
      initial_size = 16;
    }
    for (int i = 0; i < initial_size; i++) {
      ptr_initial[i] = initial[i];
    }
  }
}
