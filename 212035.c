  ~TraceProfiler() override {
    if (m_successful) {
      free(m_traceBuffer);
      IniSetting::Unbind("profiler.max_trace_buffer");
      pthread_mutex_unlock(&s_inUse);
    }
  }