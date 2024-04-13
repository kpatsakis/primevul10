  void popFrame(TraceIterator tIt, Stats& stats) {
    Frame callee = m_stack.back();
    m_stack.pop_back();
    Frame& caller = m_stack.back();
    char *cp = m_arcBuff;
    memcpy(cp, caller.trace->symbol, caller.len);
    cp += caller.len;
    if (caller.level >= 1) {
      std::pair<char*, int>& lvl = m_recursion[caller.level];
      memcpy(cp, lvl.first, lvl.second);
      cp += lvl.second;
    }
    memcpy(cp, HP_STACK_DELIM, HP_STACK_DELIM_LEN);
    cp += HP_STACK_DELIM_LEN;
    memcpy(cp, callee.trace->symbol, callee.len);
    cp += callee.len;
    if (callee.level >= 1) {
      std::pair<char*, int>& lvl = m_recursion[callee.level];
      memcpy(cp, lvl.first, lvl.second);
      cp += lvl.second;
    }
    *cp = 0;
    incStats(m_arcBuff, tIt, callee, stats);
  }