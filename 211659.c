bool Strobelight::active() {
  if (rds::isFullyInitialized() && isXenonActive()) {
    // if Xenon owns this request, back off
    return false;
  }

  // return true if a USDT probe function is listening
  return FOLLY_SDT_IS_ENABLED(hhvm, hhvm_stack);
}