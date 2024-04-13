String HHVM_FUNCTION(lcfirst,
                     const String& str) {
  return stringToCaseFirst(str, tolower, islower);
}