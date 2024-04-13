  SampleProfiler() : Profiler(true) {
    struct timeval  now;
    uint64_t truncated_us;
    uint64_t truncated_tsc;

    // Init the last_sample in tsc
    m_last_sample_tsc = cpuCycles();

    // Find the microseconds that need to be truncated
    gettimeofday(&m_last_sample_time, 0);
    now = m_last_sample_time;
    hp_trunc_time(&m_last_sample_time, SAMPLING_INTERVAL);

    // Subtract truncated time from last_sample_tsc
    truncated_us  = get_us_interval(&m_last_sample_time, &now);
    truncated_tsc = truncated_us * m_MHz;
    if (m_last_sample_tsc > truncated_tsc) {
      // just to be safe while subtracting unsigned ints
      m_last_sample_tsc -= truncated_tsc;
    }

    // Convert sampling interval to ticks
    m_sampling_interval_tsc = SAMPLING_INTERVAL * m_MHz;
  }