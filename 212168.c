  void moduleInit() override {
#ifdef CLOCK_REALTIME
    HHVM_RC_INT_SAME(CLOCK_REALTIME);
#endif
#ifdef CLOCK_MONOTONIC
    HHVM_RC_INT_SAME(CLOCK_MONOTONIC);
#endif
#ifdef CLOCK_PROCESS_CPUTIME_ID
    HHVM_RC_INT_SAME(CLOCK_PROCESS_CPUTIME_ID);
#endif
#ifdef CLOCK_THREAD_CPUTIME_ID
    HHVM_RC_INT_SAME(CLOCK_THREAD_CPUTIME_ID);
#endif
  }