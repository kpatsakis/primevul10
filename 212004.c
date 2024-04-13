Variant HHVM_FUNCTION(mcrypt_get_key_size, const String& cipher,
                                           const String& module) {
  MCRYPT td = mcrypt_module_open((char*)cipher.data(),
                                 (char*)MCG(algorithms_dir).data(),
                                 (char*)module.data(),
                                 (char*)MCG(modes_dir).data());
  if (td == MCRYPT_FAILED) {
    MCRYPT_OPEN_MODULE_FAILED("mcrypt_get_key_size");
    return false;
  }

  int64_t ret = mcrypt_enc_get_key_size(td);
  mcrypt_module_close(td);
  return ret;
}