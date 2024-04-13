ALWAYS_INLINE Variant preg_return_no_error(Variant&& return_value) {
  *rl_last_error_code = PHP_PCRE_NO_ERROR;
  return std::move(return_value);
}