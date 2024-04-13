add_bytes(regex_t* reg, UChar* bytes, int len)
{
  BB_ADD(reg, bytes, len);
  return 0;
}