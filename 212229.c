int64_t HHVM_FUNCTION(mcrypt_module_get_algo_block_size,
                                   const String& algorithm,
                                   const String& lib_dir /* = null_string */) {
  String dir = lib_dir.empty() ? String(MCG(algorithms_dir)) : lib_dir;
  return mcrypt_module_get_algo_block_size((char*)algorithm.data(),
                                           (char*)dir.data());
}