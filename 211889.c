  void doTrace(const char *symbol, bool isFuncExit) {
    TraceEntry *te = nextTraceEntry();
    if (te != nullptr) {
      collectStats(symbol, isFuncExit, *te);
    }
  }