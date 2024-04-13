copy_attr (char const *src_path, char const *dst_path)
{
  struct error_context ctx =
  {
    .error = copy_attr_error,
    .quote = copy_attr_quote,
    .quote_free = copy_attr_free
  };
  return attr_copy_file (src_path, dst_path, copy_attr_check, &ctx);
}