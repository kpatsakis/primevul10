void Strobelight::init() {
#if !defined(__APPLE__) && !defined(_MSC_VER)
  signal(strobelight::kSignumCurrent, onStrobelightSignal);
  sync_signal(strobelight::kSignumAll, onStrobelightSignal);
#endif
}