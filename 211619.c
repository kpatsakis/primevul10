  void collectStats(TraceData& te) {
    te.wall_time = cpuCycles();
    te.cpu = 0;
    if (m_flags & TrackCPU) {
      te.cpu = cpuTime(m_MHz);
    }
    if (m_flags & TrackMemory) {
      auto const stats = tl_heap->getStats();
      te.memory = stats.usage();
      te.peak_memory = stats.peakUsage;
    } else if (m_flags & TrackMalloc) {
      te.memory = get_allocs();
      te.peak_memory = get_frees();
    } else {
      te.memory = 0;
      te.peak_memory = 0;
    }
  }