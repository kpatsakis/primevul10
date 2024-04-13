main (void)
{
  FILE *f;
  char *line;
  size_t len;
  ssize_t result;

  /* Create test file.  */
  f = fopen ("test-getline.txt", "wb");
  if (!f || fwrite ("a\nA\nbc\nd\0f", 1, 10, f) != 10 || fclose (f) != 0)
    {
      fputs ("Failed to create sample file.\n", stderr);
      remove ("test-getline.txt");
      return 1;
    }
  f = fopen ("test-getline.txt", "rb");
  if (!f)
    {
      fputs ("Failed to reopen sample file.\n", stderr);
      remove ("test-getline.txt");
      return 1;
    }

  /* Test initial allocation, which must include trailing NUL.  */
  line = NULL;
  len = 0;
  result = getline (&line, &len, f);
  ASSERT (result == 2);
  ASSERT (strcmp (line, "a\n") == 0);
  ASSERT (2 < len);
  free (line);

  /* Test initial allocation again, with line = NULL and len != 0.  */
  line = NULL;
  len = (size_t)(~0) / 4;
  result = getline (&line, &len, f);
  ASSERT (result == 2);
  ASSERT (strcmp (line, "A\n") == 0);
  ASSERT (2 < len);
  free (line);

  /* Test growth of buffer, must not leak.  */
  line = malloc (1);
  len = 0;
  result = getline (&line, &len, f);
  ASSERT (result == 3);
  ASSERT (strcmp (line, "bc\n") == 0);
  ASSERT (3 < len);

  /* Test embedded NULs and EOF behavior.  */
  result = getline (&line, &len, f);
  ASSERT (result == 3);
  ASSERT (memcmp (line, "d\0f", 4) == 0);
  ASSERT (3 < len);

  result = getline (&line, &len, f);
  ASSERT (result == -1);

  free (line);
  fclose (f);
  remove ("test-getline.txt");
  return 0;
}