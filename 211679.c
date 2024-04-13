void Strobelight::surpriseAll() {
  RequestInfo::ExecutePerRequest(
    [] (RequestInfo* t) {
      // TODO: get a dedicated surprise flag to avoid colliding with xenon
      // Set the strobelight flag to collect a sample
      // TODO: isXenonActive() needs to check the request thread and not the
      // current thread (which may not even be a request)
      if (!isXenonActive()) {
        // Xenon has first crack at profiling requests. If a request
        // is marked as being profiled, we do not allow strobelight to
        // interfere with Xenon's profiling. In practice, collisions
        // should be extremely rare.
        t->m_reqInjectionData.setFlag(XenonSignalFlag);
      }
    }
  );
}