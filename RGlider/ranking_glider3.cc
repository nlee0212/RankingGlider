//Hawkeye Cache Replacement Tool v2.0
//UT AUSTIN RESEARCH LICENSE (SOURCE CODE)
//The University of Texas at Austin has developed certain software and documentation that it desires to
//make available without charge to anyone for academic, research, experimental or personal use.
//This license is designed to guarantee freedom to use the software for these purposes. If you wish to
//distribute or make other use of the software, you may purchase a license to do so from the University of
//Texas.
///////////////////////////////////////////////
//                                            //
//     Hawkeye [Jain and Lin, ISCA' 16]       //
//     Akanksha Jain, akanksha@cs.utexas.edu  //
//                                            //
///////////////////////////////////////////////

// Source code for configs 1 and 2

#include "../inc/champsim_crc2.h"
#include <map>

#define NUM_CORE 1
#define LLC_SETS NUM_CORE*2048
#define LLC_WAYS 16

//Per-set timers; we only use 64 of these
//Budget = 64 sets * 1 timer per set * 10 bits per timer = 80 bytes
#define TIMER_SIZE 1024
uint64_t perset_mytimer[LLC_SETS];

// Signatures for sampled sets; we only use 64 of these
// Budget = 64 sets * 16 ways * 12-bit signature per line = 1.5B
uint64_t signatures[LLC_SETS][LLC_WAYS];
bool prefetched[LLC_SETS][LLC_WAYS];

// Hawkeye Predictors for demand and prefetch requests
// Predictor with 2K entries and 5-bit counter per entry
// Budget = 2048*5/8 bytes = 1.2KB
#define MAX_SHCT 31
#define SHCT_SIZE_BITS 11
#define SHCT_SIZE (1<<SHCT_SIZE_BITS)
//#include "hawkeye_predictor.h"
//HAWKEYE_PC_PREDICTOR* demand_predictor;  //Predictor
//HAWKEYE_PC_PREDICTOR* prefetch_predictor;  //Predictor

#include "ranking_svm.h"
Integer_Ranking_SVM* demand_predictor;  //Predictor
Integer_Ranking_SVM*  prefetch_predictor;  //Predictor

#define OPTGEN_VECTOR_SIZE 128
#include "optgen.h"
OPTgen perset_optgen[LLC_SETS]; // per-set occupancy vectors; we only use 64 of these

#include <math.h>
#define bitmask(l) (((l) == 64) ? (unsigned long long)(-1LL) : ((1LL << (l))-1LL))
#define bits(x, i, l) (((x) >> (i)) & bitmask(l))
//Sample 64 sets per core
#define SAMPLED_SET(set) (bits(set, 0 , 6) == bits(set, ((unsigned long long)log2(LLC_SETS) - 6), 6) )

// Sampler to track 8x cache history for sampled sets
// 2800 entris * 4 bytes per entry = 11.2KB
#define SAMPLED_CACHE_SIZE 2800
#define SAMPLER_WAYS 8
#define SAMPLER_SETS SAMPLED_CACHE_SIZE/SAMPLER_WAYS
vector<map<uint64_t, ADDR_INFO> > addr_history; // Sampler

// PC histories for both Current and Victim
#define k 5 // k-sparse binary feature for PC history
#define MAX_PC_NUM 3000 // maximum number of PCs referring to Table 2.
uint64_t curr_pc_hist[k];
uint64_t victim_pc_hist[k];
uint64_t cache_pc_hist[LLC_SETS][LLC_WAYS][k];
short int binary_feature[MAX_PC_NUM];
short int curr_pc_hist_lru[k];



// initialize replacement state
void InitReplacementState()
{
    for (int i=0; i<LLC_SETS; i++) {
        for (int j=0; j<LLC_WAYS; j++) {
            signatures[i][j] = 0;
            prefetched[i][j] = false;
            for (int l=0; l<k; l++) {
                cache_pc_hist[i][j][l] = 0;
            }
        }
        perset_mytimer[i] = 0;
        perset_optgen[i].init(LLC_WAYS-2);
    }

    addr_history.resize(SAMPLER_SETS);
    for (int i=0; i<SAMPLER_SETS; i++) 
        addr_history[i].clear();

    demand_predictor = new Integer_Ranking_SVM();
    prefetch_predictor = new Integer_Ranking_SVM();

    //ranking_glider_predictor = new Integer_Ranking_SVM();
    
    for(int i=0;i<k;i++){
        curr_pc_hist[i] = 0;
        curr_pc_hist_lru[i] = -1;
        victim_pc_hist[i] = 0;
    }

    for(int i=0;i<MAX_PC_NUM;i++)
        binary_feature[i] = 0;

    cout << "Initialize Ranking Glider state" << endl;
}

// replace PC with max LRU
void replace_pc_history_lru(uint64_t PC, uint64_t* pc_history, short int* lru){
    //short int max_lru = -1;
    short int max_lru_idx = -1;
    short int empty_idx = -1;

    for(int i=0;i<k;i++){
        // if there is an empty seat, no need to find max lru
        if(lru[i] < 0){
            empty_idx = i;
            break;
        }

        // if oldest lru found, break
        if(lru[i] == k-1){
            //max_lru = lru[i];
            max_lru_idx = i;
            break;
        }
    }

    // if there was an empty seat, fill in the PC, update lru to 0 and return
    if(empty_idx != -1){
        pc_history[empty_idx] = PC;
        lru[empty_idx] = 0;
        return;
    }

    // if there is no empty seat, there should be a seat with lru == 4 (oldest)
    assert(max_lru_idx != -1);

    pc_history[max_lru_idx] = PC;
    lru[max_lru_idx] = 0;
}

// update lru value of pc history
void update_pc_history_lru(short int* lru){
    for(int i=0;i<k;i++)
        if(lru[i] >= 0)
            lru[i]++;
}

// find replacement victim
// return value should be 0 ~ 15 or 16 (bypass)
uint32_t GetVictimInSet (uint32_t cpu, uint32_t set, const BLOCK *current_set, uint64_t PC, uint64_t paddr, uint32_t type)
{
    // TODO : Implement based on predictor
    update_pc_history_lru(curr_pc_hist_lru);
    replace_pc_history_lru(PC, curr_pc_hist, curr_pc_hist_lru);


    // We cannot find a better line, we find at least better one.
    int32_t max_prediction = -10000;
    int32_t curr_prediction;
    int32_t lru_victim = -1;
    for (uint32_t i=0; i<LLC_WAYS; i++) {
        if (prefetched[set][i]) {
            curr_prediction = prefetch_predictor->get_value(curr_pc_hist, cache_pc_hist[set][i]);
            if (curr_prediction > max_prediction) {
                max_prediction = curr_prediction;
                lru_victim = i;
            }
        }
        else {
            curr_prediction = demand_predictor->get_value(curr_pc_hist, cache_pc_hist[set][i]);
            if (curr_prediction > max_prediction) {
                max_prediction = curr_prediction;
                lru_victim = i;
            }
        }
    }

    for (uint32_t j=0; j<k; j++) {
        victim_pc_hist[j] = cache_pc_hist[set][lru_victim][j];
        cache_pc_hist[set][lru_victim][j] = curr_pc_hist[j];
    }

    if( SAMPLED_SET(set) && max_prediction < 0)
    {
        //ranking_glider_predictor->decrement(curr_pc_hist,victim_pc_hist);
        if(prefetched[set][lru_victim])
            prefetch_predictor->decrement(curr_pc_hist,victim_pc_hist);
        else
            demand_predictor->decrement(curr_pc_hist,victim_pc_hist);
    }
    return lru_victim;
}

void replace_addr_history_element(unsigned int sampler_set)
{
    uint64_t lru_addr = 0;
    
    for(map<uint64_t, ADDR_INFO>::iterator it=addr_history[sampler_set].begin(); it != addr_history[sampler_set].end(); it++)
    {
   //     uint64_t timer = (it->second).last_quanta;

        if((it->second).lru == (SAMPLER_WAYS-1))
        {
            //lru_time =  (it->second).last_quanta;
            lru_addr = it->first;
            break;
        }
    }

    addr_history[sampler_set].erase(lru_addr);
}

void update_addr_history_lru(unsigned int sampler_set, unsigned int curr_lru)
{
    for(map<uint64_t, ADDR_INFO>::iterator it=addr_history[sampler_set].begin(); it != addr_history[sampler_set].end(); it++)
    {
        if((it->second).lru < curr_lru)
        {
            (it->second).lru++;
            assert((it->second).lru < SAMPLER_WAYS); 
        }
    }
}


// called on every cache hit and cache fill
void UpdateReplacementState (uint32_t cpu, uint32_t set, uint32_t way, uint64_t paddr, uint64_t PC, uint64_t victim_addr, uint32_t type, uint8_t hit)
{
    paddr = (paddr >> 6) << 6;

    if(type == PREFETCH)
    {
        if (!hit)
            prefetched[set][way] = true;
    }
    else
        prefetched[set][way] = false;

    //Ignore writebacks
    if (type == WRITEBACK)
        return;

    //If we are sampling, OPTgen will only see accesses from sampled sets
    if(SAMPLED_SET(set))
    {
        //The current timestep 
        uint64_t curr_quanta = perset_mytimer[set] % OPTGEN_VECTOR_SIZE;

        uint32_t sampler_set = (paddr >> 6) % SAMPLER_SETS; 
        uint64_t sampler_tag = CRC(paddr >> 12) % 256;
        assert(sampler_set < SAMPLER_SETS);

        // This line has been used before. Since the right end of a usage interval is always 
        // a demand, ignore prefetches
        if((addr_history[sampler_set].find(sampler_tag) != addr_history[sampler_set].end()) && (type != PREFETCH))
        {
            unsigned int curr_timer = perset_mytimer[set];
            if(curr_timer < addr_history[sampler_set][sampler_tag].last_quanta)
                curr_timer = curr_timer + TIMER_SIZE;
            bool wrap =  ((curr_timer - addr_history[sampler_set][sampler_tag].last_quanta) > OPTGEN_VECTOR_SIZE);
            uint64_t last_quanta = addr_history[sampler_set][sampler_tag].last_quanta % OPTGEN_VECTOR_SIZE;
            //and for prefetch hits, we train the last prefetch trigger PC
            if( !wrap && perset_optgen[set].should_cache(curr_quanta, last_quanta))
            {
                if(addr_history[sampler_set][sampler_tag].prefetched)
                    //prefetch_predictor->increment(addr_history[sampler_set][sampler_tag].PC);
                    prefetch_predictor->increment(curr_pc_hist,victim_pc_hist);
                else
                    //demand_predictor->increment(addr_history[sampler_set][sampler_tag].PC);
                    demand_predictor->increment(curr_pc_hist,victim_pc_hist);
            }
            else
            {
                //Train the predictor negatively because OPT would not have cached this line
                if(addr_history[sampler_set][sampler_tag].prefetched)
                    //prefetch_predictor->decrement(addr_history[sampler_set][sampler_tag].PC);
                    prefetch_predictor->decrement(curr_pc_hist,victim_pc_hist);
                else
                    //demand_predictor->decrement(addr_history[sampler_set][sampler_tag].PC);
                    demand_predictor->decrement(curr_pc_hist,victim_pc_hist);
            }
            //Some maintenance operations for OPTgen
            perset_optgen[set].add_access(curr_quanta);
            update_addr_history_lru(sampler_set, addr_history[sampler_set][sampler_tag].lru);

            //Since this was a demand access, mark the prefetched bit as false
            addr_history[sampler_set][sampler_tag].prefetched = false;
        }
        // This is the first time we are seeing this line (could be demand or prefetch)
        else if(addr_history[sampler_set].find(sampler_tag) == addr_history[sampler_set].end())
        {
            // Find a victim from the sampled cache if we are sampling
            if(addr_history[sampler_set].size() == SAMPLER_WAYS) 
                replace_addr_history_element(sampler_set);

            assert(addr_history[sampler_set].size() < SAMPLER_WAYS);
            //Initialize a new entry in the sampler
            addr_history[sampler_set][sampler_tag].init(curr_quanta);
            //If it's a prefetch, mark the prefetched bit;
            if(type == PREFETCH)
            {
                addr_history[sampler_set][sampler_tag].mark_prefetch();
                perset_optgen[set].add_prefetch(curr_quanta);
            }
            else
                perset_optgen[set].add_access(curr_quanta);
            update_addr_history_lru(sampler_set, SAMPLER_WAYS-1);
        }
        else //This line is a prefetch
        {
            assert(addr_history[sampler_set].find(sampler_tag) != addr_history[sampler_set].end());
            //if(hit && prefetched[set][way])
            uint64_t last_quanta = addr_history[sampler_set][sampler_tag].last_quanta % OPTGEN_VECTOR_SIZE;
            if (perset_mytimer[set] - addr_history[sampler_set][sampler_tag].last_quanta < 5*NUM_CORE) 
            {
                if(perset_optgen[set].should_cache(curr_quanta, last_quanta))
                {
                    if(addr_history[sampler_set][sampler_tag].prefetched)
                        //prefetch_predictor->increment(addr_history[sampler_set][sampler_tag].PC);
                        prefetch_predictor->increment(curr_pc_hist,victim_pc_hist);
                    else
                        //demand_predictor->increment(addr_history[sampler_set][sampler_tag].PC);
                        demand_predictor->increment(curr_pc_hist,victim_pc_hist);
                }
            }

            //Mark the prefetched bit
            addr_history[sampler_set][sampler_tag].mark_prefetch(); 
            //Some maintenance operations for OPTgen
            perset_optgen[set].add_prefetch(curr_quanta);
            update_addr_history_lru(sampler_set, addr_history[sampler_set][sampler_tag].lru);
        }

        // Get Hawkeye's prediction for this line
        bool new_prediction = demand_predictor->get_prediction (curr_pc_hist,victim_pc_hist);
        if (type == PREFETCH)
            new_prediction = prefetch_predictor->get_prediction (curr_pc_hist,victim_pc_hist);
        // Update the sampler with the timestamp, PC and our prediction
        // For prefetches, the PC will represent the trigger PC
        addr_history[sampler_set][sampler_tag].update(perset_mytimer[set], PC, new_prediction);
        addr_history[sampler_set][sampler_tag].lru = 0;
        //Increment the set timer
        perset_mytimer[set] = (perset_mytimer[set]+1) % TIMER_SIZE;
    }
}

// use this function to print out your own stats on every heartbeat 
void PrintStats_Heartbeat()
{

}

// use this function to print out your own stats at the end of simulation
void PrintStats()
{
    unsigned int hits = 0;
    unsigned int accesses = 0;
    for(unsigned int i=0; i<LLC_SETS; i++)
    {
        accesses += perset_optgen[i].access;
        hits += perset_optgen[i].get_num_opt_hits();
    }

    std::cout << "OPTgen accesses: " << accesses << std::endl;
    std::cout << "OPTgen hits: " << hits << std::endl;
    std::cout << "OPTgen hit rate: " << 100*(double)hits/(double)accesses << std::endl;

    cout << endl << endl;
    return;
}
