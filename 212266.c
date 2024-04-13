String stringToCaseFirst(const String& str, OpTo tocase, OpIs iscase) {
  if (str.empty() || iscase(str[0])) {
    return str;
  }

  String ret(str, CopyString);
  char* first = ret.get()->mutableData();

  *first = tocase(*first);
  return ret;
}