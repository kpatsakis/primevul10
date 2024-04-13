bool HHVM_FUNCTION(mcrypt_module_close, const Resource& td) {
  auto pm = get_valid_mcrypt_resource(td);
  if (!pm) {
    return false;
  }

  pm->close();
  return true;
}