void Strobelight::log(c_WaitableWaitHandle* wh) const {
  if (RuntimeOption::XenonForceAlwaysOn) {
    // Disable strobelight if Xenon forced on
    // TODO remove this when strobelight has its own surpriseFlag
    return;
  }

  if (getSurpriseFlag(XenonSignalFlag)) {
    // TODO remove this when strobelight has its own surpriseFlag
    clearSurpriseFlag(XenonSignalFlag);
  }

  TRACE(1, "Strobelight::log\n");
  if (active()) {
    // TODO We should filter only to hhvm samples which directly
    // caused a PMU event to fire. This is doable by storing hhvm
    // request IDs in a bpf map and checking for an entry here.
    auto bt = createBacktrace(BacktraceArgs()
                              .fromWaitHandle(wh)
                              // TODO
                              // .withMetadata()
                              .ignoreArgs());
    logToUSDT(bt);
  }
}