// Customer ID=13943; Build=0x75f5e; Copyright (c) 2015-2017 Cadence Design Systems, Inc.  ALL RIGHTS RESERVED.
// 
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Cadence Design Systems, Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Cadence Design Systems, Inc.

#include <cstdlib>
#include <stdexcept>
#include <ostream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <string>
#include <string.h>

#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>

#include "common.h"

using namespace std;


typedef unsigned long long  u64;
typedef unsigned int        u32;
typedef unsigned char       u8;


vector<string> shared_memory_names;     // shared memory names
u8 **page_table = NULL;                 // shared memory of each core
u32 *log_table  = NULL;                 // keep track of which results have been logged.  

u32 result_cnt = 0;                     // count how many results were obtained
u64 full_cntr  = 0;                     // count how many times we polled for full
u64 empty_cntr = 0;                     // count how many times we polled for empty



// Print usage info  
void usage(char *argv0) {
  cerr << "Usage: " << argv0 << " NumTasks NumCores" << endl;
  cerr << "Where:" << endl;
  cerr << "   NumTasks > 0" << endl;
  cerr << "   NumCores > 0" << endl;
  cerr << "See README.txt" << endl;
}



// Get current time as text
void timestamp(ostream &os) {
  time_t now = time(0);
  tm* localtm = localtime(&now);
  os << asctime(localtm);
}



// Create shared memory
void create_shared_memory(u32 num_cores) {
  page_table = new u8*[num_cores];
  u32 memory_fill_byte = 0;
  ostringstream oss;
  struct passwd *p_passwd = getpwuid(getuid());
  if (p_passwd) {
    oss << p_passwd->pw_name;
  }
  else {
    const char *p_user = getenv("USER");
    oss << (p_user ? p_user : "UNKNOWN_COWBOY");
  }
  string username(oss.str());
  string prefix("/");
  for (u32 N=0; N<num_cores; ++N) {
    ostringstream shm;
    shm << prefix << username << ".shmem" << N;    // For example:   joeuser.shmem0
    shared_memory_names.push_back(shm.str());
    page_table[N] = NULL;
  }
  for (u32 N=0; N<num_cores; ++N) {
    string sm_name(shared_memory_names[N]);
    int fd = shm_open(sm_name.c_str(), O_RDWR | O_CREAT | O_EXCL, 0666);
    if (fd < 0) {
      ostringstream oss;
      oss << "Could not create shared_memory_names[" << N << "]=\"" << sm_name << "\" (Hint: check /dev/shm to see if it already exists)";
      throw runtime_error(oss.str());
    }
    ftruncate(fd, SHMEM_SIZE);
    page_table[N] = (u8 *) mmap(NULL, SHMEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    if (!page_table[N]) {
      ostringstream oss;
      oss << "Could not mmap shared_memory_names[" << N << "]=\"" << sm_name << "\"";
      throw runtime_error(oss.str());
    }
    memset(page_table[N], memory_fill_byte, SHMEM_SIZE);
  }
}



// Release the shared memory
int release_shared_memory(u32 num_cores) {
  int ret_code = 0;
  for (u32 N=0; N<num_cores; ++N) {
    ret_code = shm_unlink(shared_memory_names[N].c_str());
    if (ret_code != 0) {
      cerr << "shm_unlink(\"" << shared_memory_names[N] << "\") failed: errno=" << errno << "=>" << strerror(errno) << " (Hint: check /dev/shm)";
    }
  }
  return ret_code;
}



// Create log table
void create_log_table(u32 num_cores) {
  log_table  = new u32[num_cores];
  for (u32 N=0; N<num_cores; ++N) {
    log_table[N]  = 0;
  }
}



// Return byte offset into shared memory of addr
u32 shmem_offset(u32 addr) {
  return addr - SHMEM_BASE;
}



// Return true if and only if ridx/widx indicate an empty FIFO
bool empty(u32 ridx, u32 widx) {
  empty_cntr += 1;
  return (ridx == widx);
}



// Return true if and only if ridx/widx indicate a full FIFO
bool full(u32 ridx, u32 widx) {
  full_cntr += 1;
  return ((widx + 1) % BUF_CNT == ridx);
}



// Spin until some FIFO on some core becomes non-FULL.  Start looking after prev_core.
u32 get_next_available_core(u32 prev_core, u32 num_cores) {
  u32 N = prev_core;
  bool got_one = false;
  while (!got_one) {
    for (u32 i=1; (i<=num_cores) && !got_one; ++i) {
      N = (prev_core + i) % num_cores;
      volatile u8 *shmemN = page_table[N];
      u32 widx = *(u32*)&shmemN[shmem_offset(ADDR_WIDX)];
      u32 ridx = *(u32*)&shmemN[shmem_offset(ADDR_RIDX)];
      got_one = !full(ridx, widx);
    }
  }
  return N;
}



// Log any results of specified core that have not yet been logged
void log_any_results_of_core(u32 N) {
  u32 log_ridx = log_table[N];  // log_ridx = Log Read Index (i.e. ridx to be logged)
  volatile u8 *shmemN = page_table[N];
  u32 ridx = *(u32*)&shmemN[shmem_offset(ADDR_RIDX)];
  for (u32 i=0; i<BUF_CNT; ++i) {
    u32 index = (log_ridx +i) % BUF_CNT;
    log_table[N] = index;
    if (index == ridx) break;
#ifdef PRINT_TASK_INFO
    cout << "core" << N << " res[" << index << "] = 0x" << hex << setfill('0') << setw(8) << *(u32*)&shmemN[shmem_offset(RES_BASE+4*index)] << dec
         << " result_cnt=" << result_cnt << endl;
#endif
    result_cnt += 1;
  }
}



// Spin until FIFO of specified core is empty
void wait_empty(u32 N) {
  volatile u8 *shmemN = page_table[N];
  u32 widx = *(u32*)&shmemN[shmem_offset(ADDR_WIDX)];
  u32 ridx = 0;
  do {
    ridx = *(volatile u32*)&shmemN[shmem_offset(ADDR_RIDX)];
  } while (!empty(ridx, widx));
}



// Program entry
int main(int argc, char *argv[]) {

  // Get arguments and check them
  if (argc != 3)       { usage(argv[0]); return -1; }
  u32 num_tasks = (u32) atoi(argv[1]);
  if ((num_tasks < 1)) { usage(argv[0]); return -1; }
  u32 num_cores = (u32) atoi(argv[2]);
  if ((num_cores < 1)) { usage(argv[0]); return -1; }
  u32 num_words = BUF_SIZE / 4;

  // Create shared memory
  create_shared_memory(num_cores);

  // Create table of logged results
  create_log_table(num_cores);

  // Allow user to start the XTSC system(s)
  cout << "Start XTSC systems(s) then Hit Enter to begin ..." << endl;
  cin.get();

  // Get begin time
  cout << "Doing " << num_tasks << " tasks" << endl;
  ostringstream t1; timestamp(t1);
  cout << "Beg time: " << t1.str();

  // Main loop to generate data for tasks and dispatch tasks to cores
  u32 N = num_cores - 1;        // previous/current available core
  for (u32 i=0; i<num_tasks; ++i) {
    N = get_next_available_core(N, num_cores);
    log_any_results_of_core(N);
    // Generate a buffer load of data and place it in core N's shared memory
    volatile u8 *shmemN = page_table[N];
    u32 widx = *(u32*)&shmemN[shmem_offset(ADDR_WIDX)];
    u32 value = (i & 0xFFFF) << 16;
    u32 *buffer = (u32*)&shmemN[shmem_offset(BUF_BASE+BUF_SIZE*widx)];
    for (u32 j=0; j<num_words; ++j) {
      buffer[j] = value + j;
    }
#ifdef PRINT_TASK_INFO
    cout << "Task #" << i << " core" << N << " buffer[" << widx << "]" << endl;
#endif
    widx = (widx + 1) % BUF_CNT;
    *(u32*)&shmemN[shmem_offset(ADDR_WIDX)] = widx;   // Tell core N another task (i.e. another buffer load) is ready
  }

  // Wait for all tasks on all cores to complete
  for (N=0; N<num_cores; ++N) {
    wait_empty(N);
    log_any_results_of_core(N);
  }

  // Get end time and display beg/end times
  ostringstream t2; timestamp(t2);
  cout << "empty_cntr: " << empty_cntr << endl;
  cout << "full_cntr:  " << full_cntr  << endl;
  cout << "End time: " << t2.str();
  cout << "Beg time: " << t1.str();

  // Signal all cores to exit
  for (N=0; N<num_cores; ++N) {
    volatile u8 *shmemN = page_table[N];
    *(u32*)&shmemN[shmem_offset(ADDR_WIDX)] = 0xFFFFFFFF;
  }

  // Allow user to attach a zombie XTSC system (or other mechanism) to inspect final contents of shared memory
  cout << "Optionally inspect final state of shared memory then Hit Enter to finish ..." << endl;
  cin.get();

  return release_shared_memory(num_cores);
}
