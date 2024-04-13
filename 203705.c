get_sha1(char **sha1, char const *start, char const *end)
{
  unsigned int len = end - start;
  *sha1 = xmalloc (len + 1);
  memcpy (*sha1, start, len);
  (*sha1)[len] = 0;
}