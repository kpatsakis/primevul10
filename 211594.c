Strobelight& Strobelight::getInstance() noexcept {
  static Strobelight instance;
  return instance;
}