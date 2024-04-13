void Profiler::endFrame(const TypedValue *retval,
                        const char *symbol,
                        bool endMain) {
  if (m_stack) {
    // special case for main() frame that's only ended by endAllFrames()
    if (!endMain && m_stack->m_parent == nullptr) {
      return;
    }
    endFrameEx(retval, symbol);
    m_func_hash_counters[m_stack->m_hash_code]--;
    releaseFrame();
  }
}