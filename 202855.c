savestr (char const *s)
{
  return savebuf (s, strlen (s) + 1);
}