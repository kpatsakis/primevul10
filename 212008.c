  void collectStats(const char *symbol, bool isFuncExit, TraceEntry& te) {
    te.symbol = symbol;
    te.is_func_exit = isFuncExit;
    collectStats(te);
  }