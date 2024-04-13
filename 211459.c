  void walkTrace(TraceIterator begin, TraceIterator end, TraceIterator final,
                 Stats& stats) {
    TraceWalker<TraceIterator, Stats> walker;
    walker.walk(begin, end, final, stats);
  }