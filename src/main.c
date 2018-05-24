//========================================================//
//  CSE 240a Cache Lab                                    //
//                                                        //
//  Main entry point for the Cache lab                    //
//========================================================//

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cache.h"

FILE *stream;
char *buf = NULL;
size_t len = 0;

// Print out the Usage information to stderr
//
void
usage()
{
  fprintf(stderr,"Usage: cache <options> [<trace>]\n");
  fprintf(stderr,"       bunzip -kc trace.bz2 | cache <options>\n");
  fprintf(stderr," Options:\n");
  fprintf(stderr," --help                     Print this message\n");
  fprintf(stderr," --icache=sets:assoc:hit    I-cache Parameters\n");
  fprintf(stderr," --dcache=sets:assoc:hit    D-cache Parameters\n");
  fprintf(stderr," --l2cache=sets:assoc:hit   L2-cache Parameters\n");
  fprintf(stderr," --inclusive                Makes L2-cache be inclusive\n");
  fprintf(stderr," --blocksize=size           Block/Line size\n");
  fprintf(stderr," --memspeed=latency         Latency to Main Memory\n");
}

// Process an option and update the cache
// configuration variables accordingly
//
// Returns True if Successful
//
int
handle_option(char *arg)
{
  if (!strncmp(arg,"--icache=",9)) {
    sscanf(arg+9,"%u:%u:%u", &icacheSets, &icacheAssoc, &icacheHitTime);
  } else if (!strncmp(arg,"--dcache=",9)) {
    sscanf(arg+9,"%u:%u:%u", &dcacheSets, &dcacheAssoc, &dcacheHitTime);
  } else if (!strncmp(arg,"--l2cache=",10)) {
    sscanf(arg+10,"%u:%u:%u", &l2cacheSets, &l2cacheAssoc, &l2cacheHitTime);
  } else if (!strcmp(arg,"--inclusive")) {
    inclusive = TRUE;
  } else if (!strncmp(arg,"--blocksize=",12)) {
    sscanf(arg+12,"%u", &blocksize);
  } else if (!strncmp(arg,"--memspeed=",11)) {
    sscanf(arg+11,"%u", &memspeed);
  } else {
    return 0;
  }

  return 1;
}

void
printStudentInfo()
{
  printf("Student Name:   %s\n", studentName);
  printf("Student ID:     %s\n", studentID);
  printf("Student email:  %s\n", email);
}

// Print out the memory hierarchy
//
void
printCacheConfig()
{
  printf("Simulator Memory Hierarchy:\n");
  // Print I$ Configuration
  if (icacheSets) {
    printf("  I$ Configuration:\n");
    printf("    Size:  %u KB\n", icacheSets * icacheAssoc * blocksize / 1024);
    printf("    Sets:  %u\n", icacheSets);
    printf("    Assoc: %u\n", icacheAssoc);
    printf("    Lat:   %u Cycles\n", icacheHitTime);
  }
  // Print D$ Configuration
  if (dcacheSets) {
    printf("  D$ Configuration:\n");
    printf("    Size:  %u KB\n", dcacheSets * dcacheAssoc * blocksize / 1024);
    printf("    Sets:  %u\n", dcacheSets);
    printf("    Assoc: %u\n", dcacheAssoc);
    printf("    Lat:   %u Cycles\n", dcacheHitTime);
  }
  // Print L2$ Configuration
  if (l2cacheSets) {
    printf("  L2$ Configuration:\n");
    printf("    Size:  %u KB\n", l2cacheSets * l2cacheAssoc * blocksize / 1024);
    printf("    Sets:  %u\n", l2cacheSets);
    printf("    Assoc: %u\n", l2cacheAssoc);
    printf("    Lat:   %u Cycles\n", l2cacheHitTime);
    printf("    Inclusive: %s\n", inclusive ? "Yes" : "No");
  }
  printf("  Block Size: %u Bytes\n", blocksize);
  printf("  Memspeed:   %u Cycles\n", memspeed);
}

// Print out the Cache Statistics
//
void
printCacheStats()
{
  printf("Cache Statistics:\n");
  if (icacheSets) {
    printf("  total I-cache accesses:  %10lu\n", icacheRefs);
    printf("  total I-cache misses:    %10lu\n", icacheMisses);
    printf("  total I-cache penalties: %10lu\n", icachePenalties);
    if (icacheRefs > 0) {
      printf("  I-cache miss rate:   %17.2f%%\n",
          100.0*(double)icacheMisses/(double)icacheRefs);
      printf("  avg I-cache access time: %13.2f cycles\n",
          (double)((icachePenalties + icacheRefs * icacheHitTime))/icacheRefs);
    } else {
      printf("  I-cache miss rate:                -\n");
      printf("  avg I-cache access time:          -\n");
    }
  }
  if (dcacheSets) {
    printf("  total D-cache accesses:  %10lu\n", dcacheRefs);
    printf("  total D-cache misses:    %10lu\n", dcacheMisses);
    printf("  total D-cache penalties: %10lu\n", dcachePenalties);
    if (dcacheRefs > 0) {
      printf("  D-cache miss rate:   %17.2f%%\n",
          100.0*(double)dcacheMisses/(double)dcacheRefs);
      printf("  avg D-cache access time: %13.2f cycles\n",
          (double)((dcachePenalties + dcacheRefs * dcacheHitTime))/dcacheRefs);
    } else {
      printf("  D-cache miss rate:                -\n");
      printf("  avg D-cache access time:          -\n");
    }
  }
  if (l2cacheSets) {
    printf("  total L2-cache accesses: %10lu\n", l2cacheRefs);
    printf("  total L2-cache misses:   %10lu\n", l2cacheMisses);
    printf("  total L2-cache penalties:%10lu\n", l2cachePenalties);
    if (l2cacheRefs > 0) {
      printf("  L2-cache miss rate:  %17.2f%%\n",
          100.0*(double)l2cacheMisses/(double)l2cacheRefs);
      printf("  avg L2-cache access time:%13.2f cycles\n",
          (double)((l2cachePenalties + l2cacheHitTime * l2cacheRefs))
          / l2cacheRefs);
    } else {
      printf("  L2-cache miss rate:               -\n");
      printf("  avg L2-cache access time:         -\n");
    }
  }
}

// Set the defaults for the Cache Simulator
//
void
set_defaults()
{
  // Set default input stream
  stream = stdin;

  // Set default Cache Parameters
  icacheSets      = 0;
  icacheAssoc     = 0;
  icacheHitTime   = 0;
  dcacheSets      = 0;
  dcacheAssoc     = 0;
  dcacheHitTime   = 0;
  l2cacheSets     = 0;
  l2cacheAssoc    = 0;
  l2cacheHitTime  = 0;
  inclusive       = 0;
  blocksize       = 16;
  memspeed        = 50;
}

// Reads a line from the input stream and extracts the
// Address and where the mem access should be directed to (I$ or D$)
//
// Returns True if Successful 
//
int
read_mem_access(uint32_t *addr, char *i_or_d)
{
  if (getline(&buf, &len, stream) == -1) {
    return 0;
  }

  sscanf(buf,"0x%x %c\n",addr,i_or_d);

  return 1;
}

int
main(int argc, char *argv[])
{
  // Set defaults
  set_defaults();

  // Process cmdline Arguments
  for (int i = 1; i < argc; ++i) {
    if (!strcmp(argv[i],"--help")) {
      usage();
      exit(0);
    } else if (!strncmp(argv[i],"--",2)) {
      if (!handle_option(argv[i])) {
        printf("Unrecognized option %s\n", argv[i]);
        usage();
        exit(1);
      }
    } else {
      // Use as input file
      stream = fopen(argv[i], "r");
    }
  }

  // Initialize the cache
  init_cache();

  uint64_t totalRefs = 0;
  uint64_t totalPenalties = 0;
  uint32_t addr = 0;
  char i_or_d = '\0';

  // Read each memory access from the trace
  while (read_mem_access(&addr, &i_or_d)) {
    totalRefs++;
    // Direct the memory access to the appropriate cache
    if (i_or_d == 'I') {
      totalPenalties += icache_access(addr);
    } else if (i_or_d == 'D') {
      totalPenalties += dcache_access(addr);
    } else {
      fprintf(stderr,"Input Error '%c' must be either 'I' or 'D'\n", i_or_d);
      exit(1);
    }
  }

  // Print out the statistics
  printStudentInfo();
  printCacheConfig();
  printCacheStats();
  printf("Total Memory accesses:  %lu\n", totalRefs);
  printf("Total Memory penalties: %lu\n", totalPenalties);
  if (totalRefs > 0) {
    printf("avg Memory access time: %13.2f cycles\n",
        (double)totalPenalties / totalRefs);
  } else {
    printf("avg Memory access time:             -\n");
  }

  // Cleanup
  fclose(stream);
  free(buf);

  return 0;
}
