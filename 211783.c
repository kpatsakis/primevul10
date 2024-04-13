Variant HHVM_FUNCTION(mcrypt_enc_get_algorithms_name, const Resource& td) {
  auto pm = get_valid_mcrypt_resource(td);
  if (!pm) {
    return false;
  }

  char *name = mcrypt_enc_get_algorithms_name(pm->m_td);
  String ret(name, CopyString);
  mcrypt_free(name);
  return ret;
}