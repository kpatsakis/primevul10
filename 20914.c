utf16le_is_mbc_newline(const UChar* p, const UChar* end)
{
  if (p + 1 < end) {
    if (*p == 0x0a && *(p+1) == 0x00)
      return 1;
#ifdef USE_UNICODE_ALL_LINE_TERMINATORS
    if ((
#ifndef USE_CRNL_AS_LINE_TERMINATOR
         *p == 0x0d ||
#endif
         *p == 0x85) && *(p+1) == 0x00)
      return 1;

    if (*(p+1) == 0x20 && (*p == 0x29 || *p == 0x28))
      return 1;
#endif
  }
  return 0;
}