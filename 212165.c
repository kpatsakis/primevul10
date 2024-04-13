  void walk(TraceIterator begin, TraceIterator end, TraceIterator final,
            Stats& stats) {
    if (begin == end) return;
    m_recursion.push_back(std::make_pair(nullptr, 0));
    // Trim exit traces off the front of the log. These may be due to
    // the return from turning tracing on.
    std::map<const char*, unsigned> functionLevel;
    auto current = begin;
    while (current != end && !current->symbol) ++current;
    while (current != end) {
      if (!current->is_func_exit) {
        unsigned level = ++functionLevel[current->symbol];
        if (level >= m_recursion.size()) {
          constexpr size_t bufferSize = 12;
          char *level_string = new char[bufferSize];
          snprintf(level_string, bufferSize, "@%u", level);
          m_recursion.push_back(std::make_pair(level_string,
                                               strlen(level_string)));
        }
        Frame fr;
        fr.trace = current;
        fr.level = level - 1;
        fr.len = strlen(current->symbol);
        checkArcBuff(fr.len);
        m_stack.push_back(fr);
      } else if (m_stack.size() > 1) {
        validateStack(current, stats); // NB: may update m_stack.
        --functionLevel[m_stack.back().trace->symbol];
        popFrame(current, stats);
      }
      ++current;
    }
    // Close the dangling stack with the last entry. This
    // under-represents any functions still on the stack.
    --current;
    while (m_stack.size() > 1) {
      popFrame(current, stats);
    }
    // Close main() with the final data from when profiling was turned
    // off. This ensures main() represents the entire run, even if we
    // run out of log space.
    if (!m_stack.empty()) {
      assertx(strcmp(m_stack.back().trace->symbol, "main()") == 0);
      incStats(m_stack.back().trace->symbol, final, m_stack.back(), stats);
    }
    if (m_badArcCount > 0) {
      stats["(trace has mismatched calls and returns)"].count = m_badArcCount;
    }
  }