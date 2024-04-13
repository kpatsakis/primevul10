static inline uint64_t cpuTime(int64_t /*MHz*/) {
  return gettime_ns(CLOCK_THREAD_CPUTIME_ID);
}