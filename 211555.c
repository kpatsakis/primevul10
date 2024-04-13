Variant HHVM_FUNCTION(mdecrypt_generic, const Resource& td,
                                        const String& data) {
  return mcrypt_generic(td, data, true);
}