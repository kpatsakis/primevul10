  bool ensureTraceSpace() {
    bool track_realloc = false;
    if (m_traceBufferFilled) {
      m_overflowCalls++;
      return false;
    }
    int new_array_size;
    if (m_traceBufferSize == 0) {
      new_array_size = RuntimeOption::ProfilerTraceBuffer;
    } else {
      new_array_size = m_traceBufferSize *
        RuntimeOption::ProfilerTraceExpansion;
      if (m_maxTraceBuffer != 0 && new_array_size > m_maxTraceBuffer) {
        new_array_size = m_maxTraceBuffer > m_traceBufferSize ?
          m_maxTraceBuffer : m_traceBufferSize;
      }
      if (new_array_size - m_nextTraceEntry <= 5) {
        // for this operation to succeed, we need room for the entry we're
        // adding, two realloc entries, and two entries to mark the end of
        // the trace.
        m_traceBufferFilled = true;
        collectStats("(trace buffer terminated)", false,
                     m_traceBuffer[m_nextTraceEntry++]);
        return false;
      }
      track_realloc = true;
    }
    if (track_realloc) {
      collectStats("(trace buffer realloc)", false,
                   m_traceBuffer[m_nextTraceEntry++]);
    }
    {
      MemoryManager::MaskAlloc masker(*tl_heap);
      auto r = (TraceEntry*)realloc((void*)m_traceBuffer,
                                    new_array_size * sizeof(TraceEntry));

      if (!r) {
        m_traceBufferFilled = true;
        if (m_traceBuffer) {
          collectStats("(trace buffer terminated)", false,
                       m_traceBuffer[m_nextTraceEntry++]);
        }
        return false;
      }
      m_traceBufferSize = new_array_size;
      m_traceBuffer = r;
    }
    if (track_realloc) {
      collectStats("(trace buffer realloc)", true,
                   m_traceBuffer[m_nextTraceEntry++]);
    }
    return true;
  }