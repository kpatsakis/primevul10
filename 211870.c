bool HHVM_FUNCTION(mcrypt_module_is_block_mode, const String& mode,
                                   const String& lib_dir /* = null_string */) {
  String dir = lib_dir.empty() ? String(MCG(modes_dir)) : lib_dir;
  return mcrypt_module_is_block_mode((char*)mode.data(),
                                     (char*)dir.data()) == 1;
}