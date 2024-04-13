  bool isTraceSpaceAvailable() {
    // the two slots are reserved for internal use
    return m_nextTraceEntry < m_traceBufferSize - 3;
  }