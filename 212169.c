  void endAllFrames() override {
    if (m_traceBuffer && m_nextTraceEntry < m_traceBufferSize - 1) {
      collectStats(nullptr, true, m_finalEntry);
      m_traceBufferFilled = true;
    }
  }