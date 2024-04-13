String HHVM_FUNCTION(bin2hex,
                     const String& str) {
  if (str.empty()) {
    return str;
  }

  return stringForEachBuffered(
    str.size(), str,
    [&](StringBuffer& ret, const char* src, const char* /*end*/) {
      static char hexconvtab[] = "0123456789abcdef";
      ret.append(hexconvtab[(unsigned char)*src >> 4]);
      ret.append(hexconvtab[(unsigned char)*src & 15]);
    });
}