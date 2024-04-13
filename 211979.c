String HHVM_FUNCTION(str_shuffle,
                     const String& str) {
  if (str.size() <= 1) {
    return str;
  }

  String ret(str, CopyString);
  char* buf  = ret.get()->mutableData();
  int left   = ret.size();

  while (--left) {
    int idx = HHVM_FN(rand)(0, left);
    if (idx != left) {
      char temp = buf[left];
      buf[left] = buf[idx];
      buf[idx] = temp;
    }
  }
  return ret;
}