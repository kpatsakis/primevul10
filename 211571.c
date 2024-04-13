bool HHVM_FUNCTION(mcrypt_generic_deinit, const Resource& td) {
  auto pm = get_valid_mcrypt_resource(td);
  if (!pm) {
    return false;
  }

  if (mcrypt_generic_deinit(pm->m_td) < 0) {
    raise_warning("Could not terminate encryption specifier");
    return false;
  }
  pm->m_init = false;
  return true;
}