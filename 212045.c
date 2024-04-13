Variant HHVM_FUNCTION(mcrypt_enc_self_test, const Resource& td) {
  auto pm = get_valid_mcrypt_resource(td);
  if (!pm) {
    return false;
  }

  return mcrypt_enc_self_test(pm->m_td);
}