preg_return_pcre_error(int pcre_code, Variant&& return_value) {
  pcre_handle_exec_error(pcre_code);
  return std::move(return_value);
}