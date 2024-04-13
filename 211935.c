String HHVM_FUNCTION(ucwords,
                     const String& str,
                     const String& delimiters /* = " \t\r\n\f\v"*/) {
  if (str.empty()) {
    return str;
  }

  char mask[257];
  string_charmask(delimiters.c_str(), delimiters.size(), mask);
  mask[256] = 1; // special 'start of string' character

  int last = 256;
  return stringForEach<false>(str.size(), str, [&] (char c) {
      char ret = mask[last] ? toupper(c) : c;
      last = (uint8_t)c;
      return ret;
    });
}