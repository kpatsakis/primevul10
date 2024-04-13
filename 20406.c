set_bmh_search_skip_table(UChar* s, UChar* end, OnigEncoding enc ARG_UNUSED,
                          UChar skip[])
{
  int i, len;

  len = (int )(end - s);
  if (len < ONIG_CHAR_TABLE_SIZE) {
    for (i = 0; i < ONIG_CHAR_TABLE_SIZE; i++) skip[i] = len;

    for (i = 0; i < len - 1; i++)
      skip[s[i]] = len - 1 - i;

    return 0;
  }
  else {
    return ONIGERR_PARSER_BUG;
  }
}