skip_hex_digits (char const *str)
{
  char const *s;

  for (s = str; (*s >= '0' && *s <= '9') || (*s >= 'a' && *s <= 'f'); s++)
    /* do nothing */ ;
  return s == str ? NULL : s;
}