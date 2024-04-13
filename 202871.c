remove_prefix (char *p, size_t prefixlen)
{
  char const *s = p + prefixlen;
  while ((*p++ = *s++))
    /* do nothing */ ;
}