main (void)
{
  FILE *f;
  char *line;
  size_t len;
  ssize_t result;

  /* Create test file.  */
  f = fopen ("test-getdelim.txt", "wb");
  if (!f || fwrite ("anAnbcnd\0f", 1, 10, f) != 10 || fclose (f) != 0)
    {
      fputs ("Failed to create sample file.\n", stderr);
      remove ("test-getdelim.txt");
      return 1;
    }
  f = fopen ("test-getdelim.txt", "rb");
  if (!f)
    {
      fputs ("Failed to reopen sample file.\n", stderr);
      remove ("test-getdelim.txt");
      return 1;
    }

  /* Test initial allocation, which must include trailing NUL.  */
  line = NULL;
  len = 0;
  result = getdelim (&line, &len, 'n', f);
  ASSERT (result == 2);
  ASSERT (strcmp (line, "an") == 0);
  ASSERT (2 < len);
  free (line);

  /* Test initial allocation again, with line = NULL and len != 0.  */
  line = NULL;
  len = (size_t)(~0) / 4;
  result = getdelim (&line, &len, 'n', f);
  ASSERT (result == 2);
  ASSERT (strcmp (line, "An") == 0);
  ASSERT (2 < len);
  free (line);

  /* Test growth of buffer.  */
  line = malloc (1);
  len = 1;
  result = getdelim (&line, &len, 'n', f);
  ASSERT (result == 3);
  ASSERT (strcmp (line, "bcn") == 0);
  ASSERT (3 < len);

  /* Test embedded NULs and EOF behavior.  */
  result = getdelim (&line, &len, 'n', f);
  ASSERT (result == 3);
  ASSERT (memcmp (line, "d\0f", 4) == 0);
  ASSERT (3 < len);

  result = getdelim (&line, &len, 'n', f);
  ASSERT (result == -1);

  free (line);
  fclose (f);
  remove ("test-getdelim.txt");
  return 0;
}