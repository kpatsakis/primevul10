String HHVM_FUNCTION(sha1,
                     const String& str,
                     bool raw_output /* = false */) {
  return StringUtil::SHA1(str, raw_output);
}