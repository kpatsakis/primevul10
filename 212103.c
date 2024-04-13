ALWAYS_INLINE Variant preg_return_bad_regex_error(Variant&& return_value) {
  *rl_last_error_code = PHP_PCRE_BAD_REGEX_ERROR;
  return std::move(return_value);
}