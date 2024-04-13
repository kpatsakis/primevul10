utf16be_mbc_to_code(const UChar* p, const UChar* end)
{
  OnigCodePoint code;

  if (UTF16_IS_SURROGATE_FIRST(*p)) {
    if (end - p < 4) return 0;
    code = ((((p[0] - 0xd8) << 2) + ((p[1] & 0xc0) >> 6) + 1) << 16)
         + ((((p[1] & 0x3f) << 2) + (p[2] - 0xdc)) << 8)
         + p[3];
  }
  else {
    if (end - p < 2) return 0;
    code = p[0] * 256 + p[1];
  }
  return code;
}