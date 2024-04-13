bool HHVM_FUNCTION(mcrypt_enc_is_block_algorithm_mode, const Resource& td) {
  auto pm = get_valid_mcrypt_resource(td);
  if (!pm) {
    return false;
  }

  return mcrypt_enc_is_block_algorithm_mode(pm->m_td) == 1;
}