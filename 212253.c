    static void compileTimeAssertions() {
      static_assert(sizeof(TraceData) == (sizeof(uint64_t) * 4), "");
    }