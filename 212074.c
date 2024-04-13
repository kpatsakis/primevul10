inline char* StringData::mutableData() const {
  assertx(!isImmutable());
  return const_cast<char*>(data());
}