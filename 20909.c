is_valid_mbc_string(const UChar* s, const UChar* end)
{
  return onigenc_length_check_is_valid_mbc_string(ONIG_ENCODING_UTF16_BE, s, end);
}