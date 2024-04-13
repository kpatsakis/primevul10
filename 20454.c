is_good_case_fold_items_for_search(OnigEncoding enc, int slen,
                                   int n, OnigCaseFoldCodeItem items[])
{
  int i, len;
  UChar buf[ONIGENC_MBC_CASE_FOLD_MAXLEN];

  for (i = 0; i < n; i++) {
    OnigCaseFoldCodeItem* item = items + i;

    if (item->code_len != 1)    return 0;
    if (item->byte_len != slen) return 0;
    len = ONIGENC_CODE_TO_MBC(enc, item->code[0], buf);
    if (len != slen) return 0;
  }

  return 1;
}