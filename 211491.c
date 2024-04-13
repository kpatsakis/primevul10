Variant ProfilerFactory::stop() {
  if (m_profiler) {
    m_profiler->endAllFrames();

    Array ret;
    m_profiler->writeStats(ret);
    req::destroy_raw(m_profiler);
    m_profiler = nullptr;
    RequestInfo::s_requestInfo->m_profiler = nullptr;

    return ret;
  }
  return init_null();
}