append_value_to_filename (char **filename, param_token const * const value,
                          bool is_url_encoded)
{
  int original_length = strlen (*filename);
  int new_length = strlen (*filename) + (value->e - value->b);
  *filename = xrealloc (*filename, new_length+1);
  memcpy (*filename + original_length, value->b, (value->e - value->b));
  (*filename)[new_length] = '\0';
  if (is_url_encoded)
    url_unescape (*filename + original_length);
}