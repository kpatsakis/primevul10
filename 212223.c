String HHVM_FUNCTION(str_repeat,
                     const String& input,
                     int64_t multiplier) {
  if (input.empty()) {
    return input;
  }

  if (multiplier < 0) {
    SystemLib::throwRuntimeExceptionObject(
      "Second argument has to be greater than or equal to 0");
  }

  if (multiplier == 0) {
    return empty_string();
  }

  if (input.size() == 1) {
    String ret(multiplier, ReserveString);

    memset(ret.mutableData(), *input.data(), multiplier);
    ret.setSize(multiplier);
    return ret;
  }

  auto size = multiplier * size_t(input.size());
  if (multiplier >= StringData::MaxSize || size > StringData::MaxSize) {
    raiseStringLengthExceededError(size);
  }

  StringBuffer ret(input.size() * multiplier);

  while (multiplier--) {
    ret.append(input);
  }

  return ret.detach();
}