# CSE240A Cache Simulator Project

## Table of Contents
  * [Introduction](#introduction)
  * [Code Integrity](#code-integrity)
  * [Get started](#get-started)
  * [Working with Docker](#working-with-docker)
  * [Traces](#traces)
  * [Running your Cache Simulator](#running-your-cache-simulator)
  * [Implementing the Simulator](#implementing-the-simulator)
    - [Configuration](#configuration)
    - [Inclusion](#inclusion)
    - [Uninstantiated Caches](#uninstantiated-caches)
    - [Replacement Policy](#replacement-policy)
    - [Statistics](#statistics)
  * [Grading](#grading)
    - [Test Cases](#test-cases)
    - [Grade Distribution](#grade-distribution)
  * [Turn-in Instructions](#turn-in-instructions)

## Introduction

After warming up with C programming during the branch prediction project, it's time for a slightly more challenging project.

Caches are pivotal in reducing the performance gap between processors and main
memory. Many applications are memory bound with the memory hierarchy being the
main performance bottleneck.  As you have learned in class, caches leverage the
principles of locality to attempt to reduce the average memory access time.

For this project you will be implementing a memory hierarchy in a simulated
environment.  We have provided a starting framework to help you
design your memory hierarchy.  The framework (main.c) will perform all of the
command-line switches as well as the reading in of the trace files.  You will
implement the cache simulator by completing all of the TODOs in the cache.c
file.

## Academic Integrity

Please make sure you do not copy a single line of code from any source.  Not from other students, not from the web, not from anywhere.  We have very sophisticated tools to discover if you did.  This is a graduate class and we have the very highest expectations for integrity.  You should expect that if you do so, even in very small amounts, you will be caught and this might result in you leaving the program.

## Get Started

As mentioned, we provide a starting framework to help you design your caches. The source code, traces, and expected output is in our github repo. You can get it with `git clone https://github.com/prodromou87/cacheProject240A.git`.

Alternatively, you can download it from [our github page](https://github.com/prodromou87/cacheProject240A.git).

For this project we decided to provide the correct output for the two benchmarks you are given, so you can verify your results. They can be found under the `correctOutput` directory.

You have the option to write your project in C, C++ or Python. We only provide a framework written in C and we strongly recommend you use it, primarily to ensure compatibility with our autograder. 

If you decide to use some of the other supported languages, you will have to implement everything. You also have to make sure that running `make` in the src directory generates an executable named 'cache'. During grading, our script will run a `make clean`, followed by a `make` command. Make sure that this step is not going to delete your code, especially if you are writing it in Python. Finally, make sure that your project runs with the exact same commands as this document describes. Python submissions must run with `./cache`, without requiring `python ./cache`. You can submit a custom Makefile to serve the needs of your code.

## Working with Docker

For this project, we will be using the same Docker image provided for the branch prediction project, since we have the same environment dependencies. If you already have Docker and our image installed and configured, you can skip this section.

Your projects will be graded using gradescope and an automatic grader based on Docker. The compiler used by the autograder is gcc-5.4, and runs Ubuntu 16.04. You should be able to develop this project on your own machines, but in case you want to ensure compatibility with our autograder, we provide a Docker image with the same configuration.

You will first have to install Docker (for simplicity consider Docker a very lightweight VM) on your machine (or in a VM), following the instructions found [here](https://docs.docker.com/get-started/). Once installed, you can pull our image by opening a shell (for Windows machines, powershell seems to work better than the cmd prompt) and typing:

```
docker pull prodromou87/ucsd_cse240a
```

This command will download and build our docker image. It will take a while, but you only have to do this step once. To verify that you have the image, you can run `docker images` and check the the image is listed.

Once you have it, you can start an interactive shell in Docker with 

```
docker run --rm -it prodromou87/ucsd_cse240a

The --rm flag will delete the running container once 
you exit it so it doesn't keep consuming resources 
from the host machine.

The -it flag will start an interactive session so it's 
necessary if you want a shell to work with.
```


For development purposes, you will want to mount the project directory in Docker so you can see your code, compile and run it. To do that, you change the previous instruction slightly:

```
docker run --rm -it -v /path/to/project/directory:/path/
to/mount/point prodromou87/ucsd_cse240a

Windows users will have to write the path as follows (No-
  tice the lowercase 'c'):

docker run --rm -it -v //c/path/to/project/directory:/path
/to/mount/point prodromou87/ucsd_cse240a
```

If necessary, you can mount more directories with multiple `-v` flags. Once you get a shell, you can confirm the folder has been mounted with `ls /path/to/mount/point`. You can now compile and run your project following the instructions provided in this document. You can also modify the code on your host machine using your preferred editor and only switch to docker for compiling/running. You don't need to restart docker every time since changes in the mounted directory will immediately be visible in Docker.

## Traces

Your simulator will model a cache hierarchy based on traces of real programs.
Each line in the trace file contains the address of a memory access in hex as
well as where the access should be directed, either to the I$ (I) or D$ (D).

We provide one full real-program trace (~200M memory references), and one smaller (20M references) to aid in testing your project but we
strongly suggest that you create your own custom traces to use for debugging.

```
<Address> <I or D>

Sample Trace from tsman.bz2:
0x648 I
0x64c I
0x650 I
0x654 I
0x658 I
0x40868 D
0x65c I
0x660 I
0x664 I
0x668 I
```


## Running your Cache Simulator

In order to build your simulator you simply need to run `make` in the src/
directory of the project.  You can then run the program on an uncompressed
trace as follows:

`./cache <options> [<trace>]`

If no trace file is provided then the simulator will read in input from STDIN.
Some of the traces we provided are rather large when uncompressed so we have
distributed them compressed with bzip2.  If you want to run the simulator on
a compressed trace then you can do so by doing the following:
  
`bunzip2 -kc trace.bz2 | ./cache <options>`

In either case the options that can be used to change the configurations of
the memory hierarchy are as follows:

```
  --help                     Print this message
  --icache=sets:assoc:hit    I-cache Parameters
  --dcache=sets:assoc:hit    D-cache Parameters
  --l2cache=sets:assoc:hit   L2-cache Parameters
  --inclusive                Makes L2-cache be inclusive
  --blocksize=size           Block/Line size
  --memspeed=latency         Latency to Main Memory
```


## Implementing the Simulator

There are 4 methods which need to be implemented in the cache.c file.
They are: **init_cache**, **icache_access**, **dcache_access**, **l2cache_access**.

`void init_cache();`

This will be run before any memory accesses are provided to your simulator.
This is where you will initialize all data structures or values you need
for your caches.

```
uint32_t icache_access(uint32_t addr);
uint32_t dcache_access(uint32_t addr);
uint32_t l2cache_access(uint32_t addr);
```

These 3 functions are the interface to the instruction, data, and l2 caches
respectively.  You will be given an address of a memory access to perform and
expected to return the time it took to perform the access in cycles.  Only
icache_access and dcache_access will be called from main.c, any calls to
l2cache_access will be done by your code if an access is passed up to the
l2 cache on misses in the instruction and data caches.

### Configuration

```
  [cache]Sets       // Number of sets in the cache
  [cache]Assoc      // Associativity of the cache
  [cache]HitTime    // Hit Time of the cache in cycles
  blocksize         // The Block or Line size
  inclusive         // Indicates if the L2 is inclusive
  memspeed          // Latency to Main Memory
```

Each cache can be configured to have a different number of Sets, Associativity
and Hit Time.  Additionally the block size of the memory system can be
configured.  The **I$**, **D$**, and **L2$** all have the same block size.  The L2 cache can be configured to be inclusive.  You are also able to set the latency of main memory.

### Inclusion

If the L2 is configured to be inclusive, then all valid lines in the D$ and I$
must be present in the L2.  This means if a line is evicted from the L2 then an
invalidation must be sent to both the data cache as well as the instruction
cache. If the L2 is not inclusive then this restriction doesn't hold and the
D$ and I$ could potentially hold valid lines not present in the L2.  Generally
you should observe worse cache performance when the L2 is configured to be
inclusive.  Why do you think that is?

### Uninstantiated Caches

If the number of sets of a cache is set to 0 then that cache won't be
instantiated for the simulation. This means that all accesses which are
directed to it should just be passed through to the next level in the memory
hierarchy. If a cache is uninstantiated then no statistics need to be
collected for that cache.

Keep in mind that the main loop will send requests to the L1 caches, even if they are not instantiated. You have two ways of addressing this: (1) modify the loop in the main.c file for this corner case -- **REALLY NOT RECOMMENDED**, or (2) code a check in your L1 `*_access()` functions to simply relay the request to the L2 if some cache is not instantiated. Same strategy applies for uninstantiated L2 caches.

Our autograder will be evaluating various combinations of uninstantiated caches.

### Replacement Policy

When evicting lines from a cache, in order to free space for a new line, you
should select a victim using the **LRU replacement policy**.

### Statistics

```
  [cache]Refs        // cache references (total # of accesses)
  [cache]Misses      // cache misses
  [cache]Penalties   // cache penalties
```

In addition to modeling the memory hierarchy you will be expected to maintain a
number of statistics for each cache in the hierarchy.  These statistics will be
used to calculate the miss rate and average access time for each cache.
Keeping track of references and misses is self explanatory.  The Penalties
statistic will keep track of the total penalty (in cycles) for a simulation.  A
penalty is defined as any extra overhead incurred by accessing the cache beyond
the hit time.  This means that the penalty does not take into account the
latency of an access due to a cache hit.  

As an example, let's say I have an I$ with a 2 cycle hit latency.  If I have an access which hits in the cache, no penalty is observed and I will return a 2 cycle access time.  On the other hand if I have an access which misses the I$ and is passed up to the L2 then the penalty which the I$ observes is the L2 access time.  For this access I will return the 2 cycle hit time plus the additional penalty as the I$ access time.

This means that the access time that your cache access functions will return will be the Hit Time plus any additional penalty observed.

## Grading

All grading will be done with respect to your simulator's cache statistics over
a wide range of input parameters. This means that you should make sure
that your simulator works for the various corner cases that exist. We will use four benchmarks for testing, none of which is provided in the github repo. 

You should do most of the development on your own machine. If you face any issues when you submit your project in gradescope, try to run your project in our Docker image to ensure compatibility with the autograder, or post the error message in Piazza.

### Test Cases

The autograder is designed to test your simulator (loosely-)based on real hardware. We list the hardware we will simulate, along with their technical reference manuals wherever applicable.

**NOTE:** Reference manuals are a wealth of information. They usually describe the architecture in extreme detail, provide diagrams and illustrations, electrical details, interface details and a lot more. However, they are much harder to read and understand than research papers, because their target audience is expected to have expert-level understanding of the subjects described. For those of you interested to learn how real processors are designed, this is the place to find everything you need.

1. **Intel Pentium III** - [Reference Manual](http://download.intel.com/design/PentiumIII/datashts/24526408.pdf): 
   * I$: 16KB, direct-mapped, 2 cycles hit latency
   * D$: 16KB, direct-mapped, 2 cycles hit latency
   * L2: 256KB, 8-way, on-chip, 10 cycles hit latency, inclusive
   * 64B block size
   * `./cache --icache=256:1:2 --dcache=256:1:2 --l2cache=512:8:10 --blocksize=64 --memspeed=100 --inclusive`
2. **ARM Cortex-A32** - [Reference Manual](https://static.docs.arm.com/100241/0001/cortex_a32_trm_100241_0001_00_en.pdf):
   * I$: 16KB, 2-way, 2 cycles hit latency
   * D$: 32KB, 4-way, 2 cycles hit latency
   * L2: 128KB, 8-way, on-chip, 10 cycles hit latency, non-inclusive
   * 64B block size
   * `./cache --icache=128:2:2 --dcache=128:4:2 --l2cache=256:8:10 --blocksize=64 --memspeed=100`
3. **MIPS R10K** - [Slides](https://web.stanford.edu/class/ee282h/handouts/Handout36.pdf):
   * I$: 32KB, 2-way, 2 cycles hit latency
   * D$: 32KB, 4-way, 2 cycles hit latency
   * L2: 128KB, 8-way, off-chip, 50 cycles hit latency, inclusive
   * 128B block size
   * `./cache --icache=128:2:2 --dcache=64:4:2 --l2cache=128:8:50 --blocksize=128 --memspeed=100 --inclusive`
4. **Alpha A21264** - [Reference Manual](http://www.ece.cmu.edu/~ece447/s13/lib/exe/fetch.php?media=21264hrm.pdf):
   * I$: 64KB, 2-way, 2 cycles hit latency
   * D$: 64KB, 4-way, 2 cycles hit latency
   * L2: 8MB, direct-mapped, off-chip, 50 cycles hit latency, inclusive
   * 64B block size
   * `./cache --icache=512:2:2 --dcache=256:4:2 --l2cache=16384:8:50 --blocksize=64 --memspeed=100 --inclusive`
5. **Specialized hardware:** Open-source FPGA design of a Bitcoin Miner (suspected memory hierarchy)
   * [Verilog Source Code - Github] (https://github.com/progranism/Open-Source-FPGA-Bitcoin-Miner)
   * Bitcoin mining is almost entirely compute-bound and makes very little use of memory. This is what makes it a great algorithm to map on an FPGA/ASIC. As a result, there are no caches, just a tiny main memory, which we will model as a L2 off-chip cache.
   * BTC miners are definitely not designed for general-purpose computing like the other entries in this list, however we will evaluate its memory structure as if it was part of a general-purpose processor.
   * I$: Uninstantiated
   * D$: Uninstantiated
   * L2: 1KB, direct-mapped, off-chip, 50 cycles hit latency
   * 128B block size
   * `./cache --icache=0:0:0 --dcache=0:0:0 --l2cache=8:1:50 --blocksize=128 --memspeed=100`

### Grade distribution

In the autograder we will be using four benchmarks, all different from those provided to you. One will be a full program trace, while the other three are 20M-reference-long phase traces. The four traces will be simulated on each one of the 5 test cases described above.

**Note:** Running all the test cases in gradescope takes about 8-10 minutes (for our implementation). Be patient when you submit code. Also, try to write somewhat efficient code because gradescope has a 20-minute limit on execution time.

**Grading Breakdown:**
* The project's maximum grade is 100. 
* Each of the above 5 machines (test cases) has a 20% weight on the final grade. In other words, if only the Alpha cache is correct (and the others get 0 points), your project score will be 20/100.
* For each test case (Intel, Arm, MIPS, Alpha, BTCMiner):
  - The autograder runs the 4 benchmarks described earlier. Each benchmark has equal weight.
  - Each test case can receive a total of 100 points (later combined to generate final grade)
  - First, we perform the same compatibility tests as we did for the branch prediction project. Then, we measure the total number of memory references reported. If it's not correct, the entire test case fails and you receive 0 points. No points provided if you pass this test
  - Once you pass the three compatibility tests, we will collect the number of accesses, number of misses, and number of penalty cycles for each cache in the test case. We also collect the average memory access time reported (total of 4 test fields for each test case). We then compare your answers against our implementation. Each test field has a 25% weight within the current test case. Based on how far your answer is from the correct one you get partial credit, similarly to the branch prediction project.

## Turn-in instructions

**DUE: June 5 2018 - Submissions after 11:59:59 PM are considered late**

Late projects are allowed for at most 3 days after the project due date.  For each day late a 10% grade penalty is automatically attached (e.g. 3 days late = 30% penalty).  After 3 days, missing projects will recieve a zero.  A project is considered late at 12:00:01 AM (Which is 1 second past Midnight).

You can submit zipped (.zip) submissions, or individual files through gradescope's submission interface. If you submit zipped submissions, make sure that your source files (*.h, *.c, Makefile) are laid out at the root of the compressed file. In other words do not compress a directory. Compressed directories will fail the autograder's compatibility tests. It will be more straightforward if you simply submit files individually.

Our autograder runs two sets of tests:

We first ensure that your code is compatible with our autograder. If your code fails any of these tests, you will be notified immediately, so don't leave the screen before you see the grading outcome. Specifically, this set of tests checks that:
  - `make clean && make` produces an executable named `cache`
  - The output produced by your executable has the expected format

Once you pass the compatibility tests, we grade the output produced by your code. You will be able to see your score on all test cases when the autograder is finished.

**Note:** Gradescope expects pass/fail tests but we will be reporting percentages. If you don't score 100%, Gradescope considers it a failed tests. Do not be concerned when you see failed tests (but be concerned if your score is low).  You may re-submit any number of times to improve your score.