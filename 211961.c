void end_profiler_frame(Profiler *p,
                        const TypedValue *retval,
                        const char *symbol) {
  p->endFrame(retval, symbol);
}