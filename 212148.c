void f_phprof_enable(int flags /* = 0 */) {
  if (RuntimeOption::EnableHotProfiler) {
    s_profiler_factory->start(ProfilerKind::Hierarchical, flags);
  }
}