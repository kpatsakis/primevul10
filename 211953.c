String stringForEach(uint32_t len, const String& str, Op action) {
  String ret = mutate ? str : String(len, ReserveString);

  auto srcSlice = str.slice();

  const char* src = srcSlice.begin();
  const char* end = srcSlice.end();

  char* dst = ret.mutableData();

  for (; src != end; ++src, ++dst) {
    *dst = action(*src);
  }

  if (!mutate) ret.setSize(len);
  return ret;
}