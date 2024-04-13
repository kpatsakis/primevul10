void f_hotprofiler_enable(int ikind) {
  auto kind = static_cast<ProfilerKind>(ikind);
  long flags = 0;
  if (kind == ProfilerKind::Hierarchical) {
    flags = NoTrackBuiltins;
  } else if (kind == ProfilerKind::Memory) {
    kind = ProfilerKind::Hierarchical;
    flags = NoTrackBuiltins | TrackMemory;
  }
  if (RuntimeOption::EnableHotProfiler) {
    s_profiler_factory->start(kind, flags);
  }
}