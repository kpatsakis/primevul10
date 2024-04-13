  void beginFrameEx(const char* /*symbol*/) override {
    HierarchicalProfilerFrame *frame =
      dynamic_cast<HierarchicalProfilerFrame *>(m_stack);
    frame->m_tsc_start = cpuCycles();

    if (m_flags & TrackCPU) {
      frame->m_vtsc_start = cpuTime(m_MHz);
    }

    if (m_flags & TrackMemory) {
      auto const stats = tl_heap->getStats();
      frame->m_mu_start  = stats.usage();
      frame->m_pmu_start = stats.peakUsage;
    } else if (m_flags & TrackMalloc) {
      frame->m_mu_start = get_allocs();
      frame->m_pmu_start = get_frees();
    }
  }