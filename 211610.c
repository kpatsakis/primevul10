Variant HHVM_FUNCTION(mcrypt_decrypt, const String& cipher, const String& key,
                                      const String& data, const String& mode,
                                      const Variant& viv /* = null_string */) {
  String iv = viv.toString();
  return php_mcrypt_do_crypt(cipher, key, data, mode, iv, true,
                             "mcrypt_decrypt");
}