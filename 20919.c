utf16be_is_mbc_ambiguous(OnigCaseFoldType flag, const UChar** pp, const UChar* end)
{
  const UChar* p = *pp;

  (*pp) += EncLen_UTF16[*p];

  if (*p == 0) {
    int c, v;

    p++;
    if (*p == 0xdf && (flag & INTERNAL_ONIGENC_CASE_FOLD_MULTI_CHAR) != 0) {
      return TRUE;
    }

    c = *p;
    v = ONIGENC_IS_UNICODE_ISO_8859_1_BIT_CTYPE(c,
		(BIT_CTYPE_UPPER | BIT_CTYPE_LOWER));

    if ((v | BIT_CTYPE_LOWER) != 0) {
      /* 0xaa, 0xb5, 0xba are lower case letter, but can't convert. */
      if (c >= 0xaa && c <= 0xba)
        return FALSE;
      else
        return TRUE;
    }
    return (v != 0 ? TRUE : FALSE);
  }

  return FALSE;
}