ALWAYS_INLINE Variant preg_return_internal_error(Variant&& return_value) {
  *rl_last_error_code = PHP_PCRE_INTERNAL_ERROR;
  return std::move(return_value);
}