try_match_header (const char *prefix,
                  size_t prefix_len,
                  char *line,
                  char **suffix)
{
  if (NULL != *suffix)
    return MHD_NO;
  while (0 != *line)
  {
    if (MHD_str_equal_caseless_n_ (prefix,
                                   line,
                                   prefix_len))
    {
      *suffix = strdup (&line[prefix_len]);
      return MHD_YES;
    }
    ++line;
  }
  return MHD_NO;
}