bool HHVM_FUNCTION(mcrypt_module_is_block_algorithm, const String& algorithm,
                                  const String& lib_dir /* = null_string */) {
  String dir = lib_dir.empty() ? String(MCG(algorithms_dir)) : lib_dir;
  return mcrypt_module_is_block_algorithm((char*)algorithm.data(),
                                          (char*)dir.data()) == 1;
}