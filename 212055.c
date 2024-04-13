bool Strobelight::isXenonActive() {
  if (RuntimeOption::XenonForceAlwaysOn) {
    return true;
  }

  bool xenonProfiled = Xenon::getInstance().getIsProfiledRequest();
  if (xenonProfiled) {
    return true;
  }

  return false;
}