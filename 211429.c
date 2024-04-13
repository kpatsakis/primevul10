String HHVM_FUNCTION(crypt,
                     const String& str,
                     const String& salt /* = "" */) {
  return StringUtil::Crypt(str, salt.c_str());
}