uintmax_from_header (const char *p, size_t s)
{
  return from_header (p, s, "uintmax_t", 0, UINTMAX_MAX, false, false);
}