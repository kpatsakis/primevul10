Variant HHVM_FUNCTION(mcrypt_get_block_size, const String& cipher,
                                             const String& mode) {
  MCRYPT td = mcrypt_module_open((char*)cipher.data(),
                                 (char*)MCG(algorithms_dir).data(),
                                 (char*)mode.data(),
                                 (char*)MCG(modes_dir).data());
  if (td == MCRYPT_FAILED) {
    MCRYPT_OPEN_MODULE_FAILED("mcrypt_get_block_size");
    return false;
  }

  int64_t ret = mcrypt_enc_get_block_size(td);
  mcrypt_module_close(td);
  return ret;
}