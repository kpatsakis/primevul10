  void endFrameEx(const TypedValue* /*retval*/,
                  const char* /*given_symbol*/) override {
    char symbol[512];
    HierarchicalProfilerFrame *frame =
      dynamic_cast<HierarchicalProfilerFrame *>(m_stack);
    frame->getStack(2, symbol, sizeof(symbol));
    CountMap &counts = m_stats[symbol];
    counts.count++;
    counts.wall_time += cpuCycles() - frame->m_tsc_start;

    if (m_flags & TrackCPU) {
      counts.cpu += cpuTime(m_MHz) - frame->m_vtsc_start;
    }

    if (m_flags & TrackMemory) {
      auto const stats = tl_heap->getStats();
      int64_t mu_end = stats.usage();
      int64_t pmu_end = stats.peakUsage;
      counts.memory += mu_end - frame->m_mu_start;
      counts.peak_memory += pmu_end - frame->m_pmu_start;
    } else if (m_flags & TrackMalloc) {
      counts.memory += get_allocs() - frame->m_mu_start;
      counts.peak_memory += get_frees() - frame->m_pmu_start;
    }
  }