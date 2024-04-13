void onStrobelightSignal(int signo) {
  if (!RuntimeOption::StrobelightEnabled) {
    // Handle the signal so we don't crash, but do nothing.
    return;
  }

  if (signo == strobelight::kSignumCurrent) {
    // sets on only current thread
    if (rds::isFullyInitialized()) {
      // Ignore threads that are not serving requests, otherwise this segfaults
      if (!Strobelight::isXenonActive()) {
        // Do not set the flag if Xenon is actively profiling this request
        setSurpriseFlag(XenonSignalFlag);
      }
    }
  }

  // surpriseAll currently has an issue where the isXenonActive() check will
  // try to access s_xenonData->getIsProfiledRequest() to check if the current
  // request is profiling. The problem is that you really want to check if the
  // request t is profiling. The current thread may not even be a request thread.
  // If we ever want to start using this signal for profiling,
  // we will need to figure out how to work around that problem.
  // if (signo == strobelight::kSignumAll) {
  //  // sets on ALL threads
  //  Strobelight::getInstance().surpriseAll();
  // }
}