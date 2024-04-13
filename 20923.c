is_valid_mbc_string(const UChar* p, const UChar* end)
{
  const UChar* end1 = end - 1;

  while (p < end1) {
    p += utf16le_mbc_enc_len(p);
  }

  if (p != end)
    return FALSE;
  else
    return TRUE;
}