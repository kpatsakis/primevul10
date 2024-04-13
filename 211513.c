  TraceEntry* nextTraceEntry() {
    if (!isTraceSpaceAvailable() && !ensureTraceSpace()) {
      return 0;
    }
    return &m_traceBuffer[m_nextTraceEntry++];
  }