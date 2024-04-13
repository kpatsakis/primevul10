Variant HHVM_FUNCTION(mcrypt_cfb, const String& cipher, const String& key,
                                  const String& data, const Variant& mode,
                                  const Variant& viv /* = null_string */) {
  raise_deprecated("Function mcrypt_cfb() is deprecated");
  String iv = viv.toString();
  return php_mcrypt_do_crypt(cipher, key, data, "cfb", iv, mode.toInt32(),
                             "mcrypt_cfb");
}