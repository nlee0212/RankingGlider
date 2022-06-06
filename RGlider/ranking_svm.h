#ifndef RANKER_H
#define RANKER_H

using namespace std;

#define value_t short signed int
#define hist_n 5
#define max_weight 16
#define min_weight -16


#include <iostream>
#include <math.h>
#include <set>
#include <vector>
#include <map>

uint64_t CRC( uint64_t _blockAddress )
{
    static const unsigned long long crcPolynomial = 3988292384ULL;
    unsigned long long _returnVal = _blockAddress;
    for( unsigned int i = 0; i < 32; i++ )
        _returnVal = ( ( _returnVal & 1 ) == 1 ) ? ( ( _returnVal >> 1 ) ^ crcPolynomial ) : ( _returnVal >> 1 );
    return _returnVal;
}

class Integer_Ranking_SVM
{
  map<uint64_t, value_t> TABLE;

  public:

  void increment (uint64_t curr_pc_hist[hist_n], uint64_t victim_pc_hist[hist_n])
  {
    for (int i = 0; i < hist_n; i++) {
      uint64_t curr_sig = CRC(curr_pc_hist[i]) % SHCT_SIZE;
      if (TABLE.find(curr_sig) == TABLE.end())
        TABLE[curr_sig] = 0;
      if (TABLE[curr_sig] != max_weight)
        TABLE[curr_sig]++;

      uint64_t victim_sig = CRC(victim_pc_hist[i]) % SHCT_SIZE;
      if (TABLE.find(victim_sig) == TABLE.end())
        TABLE[victim_sig] = 0;
      if (TABLE[victim_sig] != min_weight)
        TABLE[victim_sig]--;
    }
  }

  void decrement (uint64_t curr_pc_hist[hist_n], uint64_t victim_pc_hist[hist_n])
  {
    for (int i = 0; i < hist_n; i++) {
      uint64_t curr_sig = CRC(curr_pc_hist[i]) % SHCT_SIZE;
      if (TABLE.find(curr_sig) == TABLE.end())
        TABLE[curr_sig] = 0;
      if (TABLE[curr_sig] != min_weight)
        TABLE[curr_sig]--;

      uint64_t victim_sig = CRC(victim_pc_hist[i]) % SHCT_SIZE;
      if (TABLE.find(victim_sig) == TABLE.end())
        TABLE[victim_sig] = 0;
      if (TABLE[victim_sig] != max_weight)
        TABLE[victim_sig]++;
    }
  }

  bool get_prediction (uint64_t curr_pc_hist[hist_n], uint64_t victim_pc_hist[hist_n])
  {
    int64_t result = 0;
    for (int i = 0; i < hist_n; i++) {
      uint64_t curr_sig = CRC(curr_pc_hist[i]) % SHCT_SIZE;
      if (TABLE.find(curr_sig) != TABLE.end())
        result += TABLE[curr_sig];
      
      uint64_t victim_sig = CRC(victim_pc_hist[i]) % SHCT_SIZE;
      if (TABLE.find(victim_sig) != TABLE.end())
        result -= TABLE[victim_sig];
    }
    return (result > 0);
  }

  value_t get_value (uint64_t curr_pc_hist[hist_n], uint64_t victim_pc_hist[hist_n])
  {
    int64_t result = 0;
    for (int i = 0; i < hist_n; i++) {
      uint64_t curr_sig = CRC(curr_pc_hist[i]) % SHCT_SIZE;
      if (TABLE.find(curr_sig) != TABLE.end())
        result += TABLE[curr_sig];
      
      uint64_t victim_sig = CRC(victim_pc_hist[i]) % SHCT_SIZE;
      if (TABLE.find(victim_sig) != TABLE.end())
        result -= TABLE[victim_sig];
    }
    return result;
  }
};

#endif
