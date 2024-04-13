bool ProfilerFactory::start(ProfilerKind kind,
                            long flags,
                            bool beginFrame /* = true */) {
  if (m_profiler != nullptr) {
    return false;
  }

  switch (kind) {
  case ProfilerKind::Hierarchical:
    m_profiler = req::make_raw<HierarchicalProfiler>(flags);
    break;
  case ProfilerKind::Sample:
    m_profiler = req::make_raw<SampleProfiler>();
    break;
  case ProfilerKind::Trace:
    m_profiler = req::make_raw<TraceProfiler>(flags);
    break;
  case ProfilerKind::Memo:
    m_profiler = req::make_raw<MemoProfiler>(flags);
    break;
  case ProfilerKind::External:
    if (g_system_profiler) {
      m_profiler = g_system_profiler->getHotProfiler();
    } else if (m_external_profiler) {
      m_profiler = m_external_profiler;
    } else {
      raise_invalid_argument_warning(
        "ProfilerFactory::setExternalProfiler() not yet called");
      return false;
    }
    break;
  default:
    raise_invalid_argument_warning("level: %d", static_cast<int>(kind));
    return false;
  }
  if (m_profiler && m_profiler->m_successful) {
    // This will be disabled automatically when the thread completes the request
    HPHP::EventHook::Enable();
    RequestInfo::s_requestInfo->m_profiler = m_profiler;
    if (beginFrame) {
      m_profiler->beginFrame("main()");
    }
    return true;
  }
  req::destroy_raw(m_profiler);
  m_profiler = nullptr;
  return false;
}