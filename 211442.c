static int64_t get_cpu_frequency() {
  struct timeval start;
  struct timeval end;

  if (gettimeofday(&start, 0)) {
    perror("gettimeofday");
    return 0.0;
  }
  uint64_t tsc_start = cpuCycles();
  uint64_t tsc_end;
  volatile int i;
  // Busy loop for 5 miliseconds. Don't use usleep() here since it causes the
  // CPU to halt which will generate meaningless results.
  do {
    for (i = 0; i < 1000000; i++);
    if (gettimeofday(&end, 0)) {
      perror("gettimeofday");
      return 0.0;
    }
    tsc_end = cpuCycles();
  } while (get_us_interval(&start, &end) < 5000);

  return nearbyint((tsc_end - tsc_start) * 1.0
                                   / (get_us_interval(&start, &end)));
}