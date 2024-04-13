String HHVM_FUNCTION(strrev,
                     const String& str) {
  auto len = str.size();

  String ret(len, ReserveString);

  const char* data = str.data();
  char* dest = ret.get()->mutableData();

  for (int i = 0; i < len; ++i) {
    dest[i] = data[len - i - 1];
  }

  ret.setSize(len);
  return ret;
}