size_t copy_option_part(char_u **option, char_u *buf, size_t maxlen,
                        char *sep_chars)
{
  size_t len = 0;
  char_u  *p = *option;

  // skip '.' at start of option part, for 'suffixes'
  if (*p == '.') {
    buf[len++] = *p++;
  }
  while (*p != NUL && vim_strchr((char_u *)sep_chars, *p) == NULL) {
    // Skip backslash before a separator character and space.
    if (p[0] == '\\' && vim_strchr((char_u *)sep_chars, p[1]) != NULL) {
      p++;
    }
    if (len < maxlen - 1) {
      buf[len++] = *p;
    }
    p++;
  }
  buf[len] = NUL;

  if (*p != NUL && *p != ',') {  // skip non-standard separator
    p++;
  }
  p = skip_to_option_part(p);    // p points to next file name

  *option = p;
  return len;
}