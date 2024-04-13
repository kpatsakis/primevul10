Variant HHVM_FUNCTION(mcrypt_enc_get_iv_size, const Resource& td) {
  auto pm = get_valid_mcrypt_resource(td);
  if (!pm) {
    return false;
  }

  return mcrypt_enc_get_iv_size(pm->m_td);
}