  explicit TraceProfiler(int flags)
    : Profiler(true)
    , m_traceBuffer(nullptr)
    , m_traceBufferSize(0)
    , m_nextTraceEntry(0)
    , m_traceBufferFilled(false)
    , m_maxTraceBuffer(0)
    , m_overflowCalls(0)
    , m_flags(flags)
  {
    if (!(m_flags & IHaveInfiniteMemory) && pthread_mutex_trylock(&s_inUse)) {
      // This profiler uses a very large amount of memory. Only allow
      // one in the process at any time.
      m_successful = false;
    } else {
      m_maxTraceBuffer = RuntimeOption::ProfilerMaxTraceBuffer;
      Extension* ext = ExtensionRegistry::get(s_hotprofiler);
      assertx(ext);
      IniSetting::Bind(ext, IniSetting::PHP_INI_ALL,
                       "profiler.max_trace_buffer",
                       &m_maxTraceBuffer);
    }
  }