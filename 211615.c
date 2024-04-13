Variant HHVM_FUNCTION(mcrypt_generic, const Resource& td, const String& data) {
  return mcrypt_generic(td, data, false);
}