contains_slash (const char *s)
{
  for (; *s; s++)
    if (ISSLASH(*s))
      return true;
  return false;
}