String HHVM_FUNCTION(strtolower,
                     const String& str) {
  return stringForEachFast(str, tolower);
}