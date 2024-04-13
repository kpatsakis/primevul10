collated_compare (const void *a, const void *b)
{
  char *const *ps1 = a; char *s1 = *ps1;
  char *const *ps2 = b; char *s2 = *ps2;

  if (s1 == s2)
    return 0;
  if (s1 == NULL)
    return 1;
  if (s2 == NULL)
    return -1;
  return strcoll (s1, s2);
}