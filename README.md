
# Welcome to the 2nd Cache Replacement Championship!

If you have not already done so, plase join the CRC-2 mailing 
list by sending an empty email to:

crc-2+subscribe@googlegroups.com

You do not need to have a Google email account - you only need to 
subscribe to the mailing list. When prompted, type in your email
address that you want use for the CRC-2 subscription.

This mailing list will be used for any announcements regarding the 
competition, any simulation infrastructure updates, and participants 
may post questions or report problems.

You must provide a single .cc (or .c) file for your replacement policy.  
There are two example replacement policies (LRU and SRRIP [Jaleel et al. ISCA '10]) 
in the example directory. Refer to them to learn how to interface with 
the Champsim Simulator.

# How to compile

For the championship, your replacement policy's performance will be measured in 
four configurations:

1. Single core with 2MB LLC without a prefetcher (lib/config1.a)
2. Single core with 2MB LLC with L1/L2 data prefetchers (lib/config2.a)
3. A 4-core configuration with 8MB of shared LLC without a prefetcher (lib/config3.a)
4. A 4-core configuration with 8MB of shared LLC with L1/L2 data prefetchers (lib/config4.a)

We also provide config5 and config6 that have a larger 8MB LLC on config1 
and config2 respectively. You can use these configurations to calculate the 
normalized weighted speedup for multi-core system. Note that config5 and config6 
will not be tested in the final competition.

Compile your replacement policy file with pre-compiled libraries:
```
$ g++ -Wall --std=c++11 -o lru-config1 example/lru.cc lib/config1.a
$ g++ -Wall --std=c++11 -o lru-config2 example/lru.cc lib/config2.a
$ g++ -Wall --std=c++11 -o lru-config3 example/lru-8MB.cc lib/config3.a
$ g++ -Wall --std=c++11 -o lru-config4 example/lru-8MB.cc lib/config4.a
$ g++ -Wall --std=c++11 -o lru-config5 example/lru-8MB.cc lib/config5.a
$ g++ -Wall --std=c++11 -o lru-config6 example/lru-8MB.cc lib/config6.a
```
We used g++ (Ubuntu 4.8.4-2ubuntu1~14.04.3) to compile the example codes. 

# How to run

The ChampSim Simulator requires three parameters:

-warmup_instructions <number>
Use this to specify the length of the warmup period. After the warmup
period is over, the IPC statistics are reset, and the final reported
IPC metric will be calculated starting at this point.
Default value is 1,000,000.

-simulation_instructions <number>
Use this to specify how many instructions you want to execute after the
warmup period is over. After the simulation period is over, the simulator
will exit and IPC since the warmup period will be printed.
Default value is 10,000,000.

-hide_heartbeat
Use this to hide the heartbeat information that shows up every 1,000,000
instructions.

-traces
Use this to specify the instruction traces generated by using the ChampSim PIN tool.
For 4-core you must specify the name of trace files individually. 

For example, to run a single-core simulation using the default LRU replacement algorithm:
```
$ g++ -Wall --std=c++11 -o lru-config1 example/lru.cc lib/config1.a
$ ./lru-config1 -warmup_instructions 1000000 -simulation_instructions 10000000 -traces trace/bzip2_10M.trace.gz
```

To run a 4-core simulation using the LRU replacement algorithm:
```
$ g++ -Wall --std=c++11 -o lru-config3 example/lru-8MB.cc lib/config3.a
$ ./lru-config3 -warmup_instructions 1000000 -simulation_instructions 10000000 -traces trace/bzip2_10M.trace.gz trace/mcf_10M.trace.gz trace/libquantum_10M.trace.gz trace/xalancbmk_10M.trace.gz
```

-cloudsuite
Use this to test multi-core traces from CloudSuite. Note that this option does not 
work with the old example traces (bzip2, libquantum, mcf, xalancbmk, graph_analytics) 
included in /trace directory and other traces collected from champsim_tracer.cpp.

To run a 4-core simulation using the LRU replacement algorithm:
```
$ g++ -Wall --std=c++11 -o lru-config3 example/lru-8MB.cc lib/config3.a
$ ./lru-config3 -warmup_instructions 1000000 -simulation_instructions 10000000 -cloudsuite -traces trace/cassandra_core_0.trace.gz trace/cassandra_core_1.trace.gz trace/cassandra_core_2.trace.gz trace/cassandra_core_3.trace.gz
```

# How to evaluate your replacement policy

For CRC-2, we will test following six different test cases.

1. Single-core SPEC CPU 2006 benchmarks without prefetcher
$ g++ -Wall --std=c++11 -o lru-config1 example/lru.cc lib/config1.a
$ ./lru-config1 -warmup_instructions 1000000 -simulation_instructions 10000000 -traces trace/bzip2_10M.trace.gz

2. Single-core SPEC CPU 2006 benchmarks with prefetcher
$ g++ -Wall --std=c++11 -o lru-config1 example/lru.cc lib/config2.a
$ ./lru-config2 -warmup_instructions 1000000 -simulation_instructions 10000000 -traces trace/bzip2_10M.trace.gz

3. 4-core multi-app SPEC CPU 2006 benchmarks without prefetcher
$ g++ -Wall --std=c++11 -o lru-config3 example/lru-8MB.cc lib/config3.a
$ ./lru-config3 -warmup_instructions 1000000 -simulation_instructions 10000000 -traces trace/bzip2_10M.trace.gz trace/mcf_10M.trace.gz trace/libquantum_10M.trace.gz trace/xalancbmk_10M.trace.gz

4. 4-core multi-app SPEC CPU 2006 benchmarks with prefetcher
$ g++ -Wall --std=c++11 -o lru-config4 example/lru-8MB.cc lib/config4.a
$ ./lru-config4 -warmup_instructions 1000000 -simulation_instructions 10000000 -traces trace/bzip2_10M.trace.gz trace/mcf_10M.trace.gz trace/libquantum_10M.trace.gz trace/xalancbmk_10M.trace.gz

5. 4-core multi-thread CloudSuite benchmarks without prefetcher
$ g++ -Wall --std=c++11 -o lru-config3 example/lru-8MB.cc lib/config3.a
$ ./lru-config3 -warmup_instructions 1000000 -simulation_instructions 10000000 -cloudsuite -traces trace/cassandra_core_0.trace.gz trace/cassandra_core_1.trace.gz trace/cassandra_core_2.trace.gz trace/cassandra_core_3.trace.gz

6. 4-core multi-thread CloudSuite benchmarks with prefetcher 
$ g++ -Wall --std=c++11 -o lru-config4 example/lru-8MB.cc lib/config4.a
$ ./lru-config4 -warmup_instructions 1000000 -simulation_instructions 10000000 -cloudsuite -traces trace/cassandra_core_0.trace.gz trace/cassandra_core_1.trace.gz trace/cassandra_core_2.trace.gz trace/cassandra_core_3.trace.gz

For a single-core configuration, we will measure the geomean IPC speedup (normalized to LRU) across all benchmarks. 
For a 4-core configuration, we will measure the weighted IPC speedup (normalized to LRU) across all multi-app (multi-thread) workloads.

# How to create traces

We have included 5 example traces, four from SPEC CPU 2006 and one from CloudSuite 2.0. 
These traces are short (10 million instructions), and do not necessarily cover the range of 
behaviors your replacement algorithm will likely see in the full competition trace list 
(not included). We STRONGLY recommend creating your own traces, covering
a wide variety of program types and behaviors.

You need to download Pin 3.0 (pin-3.0-76991-gcc-linux), and may require 
installing libdwarf.so, libelf.so, or other libraries, if you do not already 
have them. Please refer to the following link to download Pin 3.0.

https://software.intel.com/en-us/articles/pin-a-dynamic-binary-instrumentation-tool
https://software.intel.com/sites/landingpage/pintool/docs/76991/Pin/html/

The included Pin Tool tracer file (trace/champsim_tracer.cpp) can be used to generate new 
traces. Compile the tracer file with trace/make_tracer.sh and use your Pin Tool as follows.

**Use the Pin tool like this**
```
$ pin -t /path/to/ChampSim/tracer/obj-intel64/champsim_tracer.so -- <your program here>
```

The tracer has three options you can set:
```
-o
Specify the output file for your trace.
The default is champsim.trace

-s <number>
Specify the number of instructions to skip in the program before tracing begins.
The default value is 0.

-t <number>
The number of instructions to trace, after -s instructions have been skipped.
The default value is 1,000,000.
```
For example, you could trace 200,000 instructions of the program ls, after
skipping the first 100,000 instructions, with this command:
```
pin -t /path/to/ChampSim/tracer/obj-intel64/champsim_tracer.so -o traces/ls.trace -s 100000 -t 200000 -- ls
gzip ls.trace
```
Traces created with the champsim_tracer.so are 64 bytes per instruction,
but they generally compress down to 2-10 bytes per instruction using gzip.
ChampSim takes a gzip file as input with -traces argument.

# From Na Yeon (small notes)

InitReplacementState(), GetVictimInSet(), UpdateReplacementState(), PrintStats_Heartbeat(), PrintStats() are all default functions.

In the LRU example, lru[LLC_SETS][LLC_WAYS] is used for keeping track of 'how recent' the 'way' is.
Here, 'sets' and 'ways' are used. Sets refer to sampled sets, and way in the UpdateReplacementState function parameter refer to 'way' that is chosen when cache hit (refer to n-way cache)

Hawkeye seems to be based on the SRRIP example. It uses Re-Reference Interval Prediction (RRIP), and utilizes 'maxRRPV'.

# From Taeyoung

(hawkeye implementation)

## Arrays
`rrpv` : Array containing RRPV value. 
`signature` : Contains PC.
`prefetched` : Denote whether this line is prefetched, or on demand.
`addr_history` : `ADDR_INFO[#Sample_Set][#Tag]`.
`ADDR_INFO` : Contains address, last quanta, PC, prefetched, lru.

## Somethings
- tag : `CRC(paddr >> 12) % 256`
- set : `(paddr >> 6) % SAMPLER_SETS`
- `SAMPLED_SET` : Hawkeye uses only 64 sets for training, and this represents such SET.

## Parameter
- `cpu` : ???
- `set` : The index of set.
- `current_set` : ???
- `PC` : Current PC
- `paddr` : Physical address. Used to get tag and set.
- `type` : The type of this access. Demand or Prefetch.
- `sampler_set` : The set that will be updated/replaced.
- `curr_lru` : LRU value that will be used to update.
- `way` : The way of victim / hit.
- `victim_addr` : The address of victim.
- `hit` : Is this update hit? or miss?

## Functions
`InitReplacementState()`
Initialize the state.

- RRPV : Init to MaxRRPV.
- Signature : Init to 0.
- Prefetchec : Init to False.

`GetVictimInSet`

1. If exist maxRRPV, let it to victim.
2. If none, find one with RRPV maximum.
3. Train predictor, decrease the victim.
4. Return the victim.

`replace_addr_history_element`
Iterate address history. Erase the element with largest LRU value.

`update_addr_history_lru`
Iterate address history. Increase lru for those with lower lru than curr_lru.

`UpdateReplacementState()`

The line is either demanded (type != PREFETCH) or prefetch (type == PREFETCH).

- The line has been used, and is not prefetch.
  + Tested using `history.find(tag) != history.end()` (line 188)
  + If OPTgen predicted to should cache this line (line 196), increment.
  + Else (line 203), decrement (207, 209).
  + Maintenance OPTgen() (line 212-213), and mark the prefetched to false.
  + Q : What does wrap do? It seems to check whether this line is far from current timer.
- The line has not been used.
  + Tested using `history.find(tag) == history.end()` (line 219) 
  + Remove ont element from addr history if full (line 223).
  + Add new entry (line 227), and add its info to OPTgen. 
- The line is prefetched.
  + Tested using `type == PREFETCH` (line 238)
  + Again, if OPTgen predicted to should cache this line, increment.
  + Mark prefetch, and maintenence on OPTgen.
- After these three branch, do following.
  + Get prediction from Hawkeye (line 262-264).
  + Update the history, and timer.
- Finally, set RRPV value.

(Our solution)

I don't think that we should modify much of the components.

1. We implement Integer Ranking SVM, which replaces `hawkeye_predictor.h`.
2. Each call of increment/decrement to `prefetch_predictor` and `demand_predictor` should be replaced to integer SVM's.
3. Such call require two parameter. Current PC history, victim's PC history. We will maintain these in global varaible.

So, we need to modify following functions.

`InitReplacementState()`
- Line 83, 84 : Modify to Ranking SVM initialization
- Add PC History for both cache and current.

`GetVictimInSet()`
- Rewrite, but similar to LRU version.
- It will access our predictor, and return first negative example.
- If no such element, return 16 (bypass).
- line 112-118 : Replace hawkeye predictor to Ranking SVM.

`UpdateReplacementState()`
- line 199, 201, 207, 209, 248, 250. Replace hawkeye predictor to Ranking SVM.
- Add the PC history maintenance.
  + For the current PC history, replace the oldest one to current PC.
  + For the cache history, 

# Nayeon
(5/31)
- Created a RGlider directory which will be our main working directory
- Renamed the file names
- Will work on function modification

(6/3)
Q1: To make sure, updating victim_pc_hist has to be done before predictor's decrement in GetVictimInSet() right?
Q2: Updating curr_pc_hist has to be done right before predictor's get_prediction in UpdateReplacementState() right?

