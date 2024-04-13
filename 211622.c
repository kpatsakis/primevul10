  void writeStats(Array &ret) override {
    TraceData my_begin;
    collectStats(my_begin);
    walkTrace(m_traceBuffer, m_traceBuffer + m_nextTraceEntry, &m_finalEntry,
              m_stats);
    if (m_overflowCalls) {
      m_stats["(trace buffer terminated)"].count += m_overflowCalls/2;
    }
    extractStats(ret, m_stats, m_flags, m_MHz);
    CountedTraceData allocStats;
    allocStats.count = 0;
    allocStats.peak_memory = allocStats.memory =
      m_nextTraceEntry * sizeof(*m_traceBuffer);
    returnVals(ret, "(trace buffer alloc)", allocStats, m_flags, m_MHz);
    if (m_flags & MeasureXhprofDisable) {
      CountedTraceData my_end;
      collectStats(my_end);
      my_end.count = 1;
      my_end.cpu -= my_begin.cpu;
      my_end.wall_time -= my_begin.wall_time;
      my_end.memory -= my_begin.memory;
      my_end.peak_memory -= my_begin.peak_memory;
      returnVals(ret, "xhprof_post_processing()", my_end, m_flags, m_MHz);
    }
  }