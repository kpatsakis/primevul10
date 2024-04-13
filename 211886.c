Variant HHVM_FUNCTION(mcrypt_get_cipher_name, const String& cipher) {
  MCRYPT td = mcrypt_module_open((char*)cipher.data(),
                                 (char*)MCG(algorithms_dir).data(),
                                 (char*)"ecb",
                                 (char*)MCG(modes_dir).data());
  if (td == MCRYPT_FAILED) {
    td = mcrypt_module_open((char*)cipher.data(),
                            (char*)MCG(algorithms_dir).data(),
                            (char*)"stream",
                            (char*)MCG(modes_dir).data());
    if (td == MCRYPT_FAILED) {
      MCRYPT_OPEN_MODULE_FAILED("mcrypt_get_cipher_name");
      return false;
    }
  }

  char *cipher_name = mcrypt_enc_get_algorithms_name(td);
  mcrypt_module_close(td);
  String ret(cipher_name, CopyString);
  mcrypt_free(cipher_name);
  return ret;
}