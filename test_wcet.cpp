#include "test_wcet.h"
#include <sched.h> // sched_setscheduler
#include <stdint.h> /* int64_t*/
#include <time.h>   /* for timestamp, link with -lrt*/
#include <unistd.h>
#include <algorithm>
#include <numeric>

struct watch {
  struct timespec ts_start;
  struct timespec ts_end;
  int64_t ts_diff;
};

int start_watch(struct watch *wch) {
  if (clock_gettime(CLOCK_MONOTONIC, &(wch->ts_start)) < 0)
    perror("clock_gettime");
  return 0;
}

int stop_watch(struct watch *wch) {
  if (clock_gettime(CLOCK_MONOTONIC, &(wch->ts_end)) < 0)
    perror("clock_gettime");
  wch->ts_diff = wch->ts_end.tv_nsec - wch->ts_start.tv_nsec +
                 (wch->ts_end.tv_sec - wch->ts_start.tv_sec) * 1000000000UL;

  return 0;
}

struct BurstTimeTest {
  std::string name;
  int64_t max_time;
  int64_t avg_time;
  int64_t min_time;
  int count;
};

bool TaskLoader::RunTest() {
  std::vector<BurstTimeTest> result;
  /*set the rt prior , with sudo */
  struct sched_param param;
  param.sched_priority = 2; /*SCHED_FIFO 2*/
  if(sched_setscheduler(getpid(), SCHED_FIFO, &param) != 0)
  {
    perror("sched_setscheduler");
  }

  struct watch runtime;
  start_watch(&runtime);
  auto checkAndSleep = [&] {
    stop_watch(&runtime);
    /* if run for over 950 ms then rest for 1 s*/
    if (runtime.ts_diff > 950000000L) {
      sleep(1); /* sleep 1 s, because RT bandwidth update interval is 1 s*/
      start_watch(&runtime);
    }
  };

  for (auto it = mTaskArr.begin(); it != mTaskArr.end(); ++it) {
    std::vector<int64_t> btVec;
    for (int i = 0; i < 3; ++i) {
      struct watch wt;
      start_watch(&wt);
      it->second.runFunc();
      stop_watch(&wt);
      btVec.emplace_back(wt.ts_diff);
      checkAndSleep();
      // usleep(10000); /* sleep 10 ms*/
    }

    result.emplace_back(BurstTimeTest{
        it->first, *std::max_element(btVec.begin(), btVec.end()),
        std::accumulate(btVec.begin(), btVec.end(), 0L) /
            (int64_t)btVec.size(),
        *std::min_element(btVec.begin(), btVec.end()), (int)btVec.size()});
  }
  std::cout << "==================result================" << std::endl;
/* print result in ms or ns*/
#ifdef UNIT_MS
  std::cout << "name"
               ", max time(ms)"
               ", avg time"
               ", min time"
               ", count"
            << std::endl;
  int64_t unit = 1000000;
  for (auto &res : result) {
    std::cout << res.name << ", " << res.max_time / unit + 1 << ", "
              << res.avg_time / unit + 1 << ", " << res.min_time / unit + 1
              << ", " << res.count << std::endl;
  }
#else
  std::cout << "name"
               ", max time"
               ", avg time"
               ", min time"
               ", count"
            << std::endl;
  for (auto &res : result) {
    std::cout << res.name << ", " << res.max_time << ", " << res.avg_time
              << ", " << res.min_time << ", " << res.count << std::endl;
  }
#endif
  return true;
}