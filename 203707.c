sha1_says_nonexistent(char const *sha1)
{
  char const *empty_sha1 = "e69de29bb2d1d6434b8b29ae775ad8c2e48c5391";
  char const *s;

  /* Nonexisting files have an all-zero checksum.  */
  for (s = sha1; *s; s++)
    if (*s != '0')
      break;
  if (! *s)
    return 2;

  /* Empty files have empty_sha1 as their checksum.  */
  for (s = sha1; *s; s++, empty_sha1++)
    if (*s != *empty_sha1)
      break;
  return ! *s;
}