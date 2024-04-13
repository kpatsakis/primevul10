add_char_amb_opt_map(OptMap* map, UChar* p, UChar* end,
                     OnigEncoding enc, OnigCaseFoldType fold_flag)
{
  OnigCaseFoldCodeItem items[ONIGENC_GET_CASE_FOLD_CODES_MAX_NUM];
  UChar buf[ONIGENC_CODE_TO_MBC_MAXLEN];
  int i, n;

  add_char_opt_map(map, p[0], enc);

  fold_flag = DISABLE_CASE_FOLD_MULTI_CHAR(fold_flag);
  n = ONIGENC_GET_CASE_FOLD_CODES_BY_STR(enc, fold_flag, p, end, items);
  if (n < 0) return n;

  for (i = 0; i < n; i++) {
    ONIGENC_CODE_TO_MBC(enc, items[i].code[0], buf);
    add_char_opt_map(map, buf[0], enc);
  }

  return 0;
}