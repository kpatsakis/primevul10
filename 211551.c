bool HHVM_FUNCTION(mcrypt_generic_end, const Resource& td) {
  return HHVM_FUNCTION(mcrypt_generic_deinit, td);
}