#ifndef Stats_H
#define Stats_H
#include <iomanip>

using namespace std;

struct Stats{
  int total_accesses{};
  int reads{};
  int writes{};
  int hits{};
  int misses{};
  int writebacks{};
  int writes_to_next_level{};

  Stats(){};

  void print(){
    string w = "  ";
    float hitRate = total_accesses > 0 ? (100.0f * hits / total_accesses) : 0.0f;
    cout << "Results:" << endl;
    cout << w << "Cache accesses:       " << total_accesses << endl;
    cout << w << "Reads:                " << reads << endl;
    cout << w << "Writes:               " << writes << endl;
    cout << w << "Hits:                 " << hits << endl;
    cout << w << "Misses:               " << misses << endl;
    cout << w << "Hit rate:             " << fixed << setprecision(2) << hitRate << "%" << endl;
    cout << w << "Writebacks:           " << writebacks << endl;
    cout << w << "Writes to next level: " << writes_to_next_level << endl;
  };
};

#endif
