set_sunday_quick_search_skip_table(UChar* s, UChar* end, OnigEncoding enc,
                                   UChar skip[], int* roffset)
{
  int i, len, offset;

  offset = 1;
  if (ONIGENC_MBC_MINLEN(enc) > 1) {
    UChar* p = s;
    while (1) {
      len = enclen(enc, p);
      if (p + len >= end) {
        UChar* q = p + (ONIGENC_MBC_MINLEN(enc) - 1);
        while (q > p) {
          if (*q != '\0') {
            offset = q - p + 1;
            break;
          }
          q--;
        }
        break;
      }
      p += len;
    }
  }

  len = (int )(end - s);
  if (len + offset >= ONIG_CHAR_TABLE_SIZE)
    return ONIGERR_PARSER_BUG;

  *roffset = offset;

  for (i = 0; i < ONIG_CHAR_TABLE_SIZE; i++) skip[i] = (UChar )(len + offset);

  for (i = 0; i < len; i++)
    skip[s[i]] = len - i + (offset - 1);

  return 0;
}