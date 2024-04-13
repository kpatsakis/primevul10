inline bool StringData::isImmutable() const {
  return !isRefCounted() || isProxy();
}