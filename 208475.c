static void fill_breakat_flags(void)
{
  char_u      *p;
  int i;

  for (i = 0; i < 256; i++)
    breakat_flags[i] = FALSE;

  if (p_breakat != NULL)
    for (p = p_breakat; *p; p++)
      breakat_flags[*p] = TRUE;
}