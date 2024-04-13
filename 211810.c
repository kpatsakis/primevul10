  void validateStack(TraceIterator tIt, Stats& stats) {
    auto enteredName = m_stack.back().trace->symbol;
    auto exitedName = tIt->symbol;
    if ((exitedName != nullptr) &&
        ((enteredName == nullptr) || (strcmp(enteredName, exitedName) != 0))) {
      // We have a few special names that we form on entry. We don't
      // have the information to form them again on exit, so tolerate
      // them here. See EventHook::GetFunctionNameForProfiler().
      if ((enteredName != nullptr) &&
          ((strncmp(enteredName, "run_init::", 10) == 0) ||
           (strcmp(enteredName, "_") == 0))) return;
      bool fixed = false;
      if (m_stack.size() > 1) {
        auto callerName = (m_stack.end() - 2)->trace->symbol;
        if ((callerName != nullptr) && (strcmp(callerName, exitedName) == 0)) {
          // We have an exit for Foo(), but we were in Bar(). However,
          // it appears that Foo() was the caller of Bar(). This
          // suggests we've missed the exit event for Bar() and have
          // the exit event for Foo() in hand. So remove Bar() to
          // re-balance the stack.
          m_stack.pop_back();
          fixed = true;
        }
      }
      // The first few bad arcs typically point at the problem, so
      // report them. The rest we'll just count.
      if (++m_badArcCount < 20) {
        std::string badArc;
        if (fixed) {
          badArc = folly::format("(warning: corrected bad arc #{}: "
                                 "enter '{}', exit '{}')",
                                 m_badArcCount,
                                 enteredName, exitedName).str();
        } else {
          badArc = folly::format("(error: bad arc #{}: "
                                 "enter '{}', exit '{}')",
                                 m_badArcCount,
                                 enteredName, exitedName).str();
        }
        ++stats[badArc.data()].count;
      }
    }
  }