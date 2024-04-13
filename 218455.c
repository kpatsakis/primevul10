fetch_token (re_token_t *result, re_string_t *input, reg_syntax_t syntax)
{
  re_string_skip_bytes (input, peek_token (result, input, syntax));
}