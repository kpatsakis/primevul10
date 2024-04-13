udequote_pathname (pathname)
     char *pathname;
{
  register int i, j;

  for (i = j = 0; pathname && pathname[i]; )
    {
      if (pathname[i] == '\\')
	i++;

      pathname[j++] = pathname[i++];

      if (pathname[i - 1] == 0)
	break;
    }
  if (pathname)
    pathname[j] = '\0';
}