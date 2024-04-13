int64_t HHVM_FUNCTION(crc32,
                      StringArg str) {
  return (uint32_t)string_crc32(str.get()->data(), str.get()->size());
}