split_ignorespec (s, ip)
     char *s;
     int *ip;
{
  char *t;
  int n, i;

  if (s == 0)
    return 0;

  i = *ip;
  if (s[i] == 0)
    return 0;

  n = skip_to_delim (s, i, ":", SD_NOJMP|SD_EXTGLOB|SD_GLOB);
  t = substring (s, i, n);

  if (s[n] == ':')
    n++;  
  *ip = n;  
  return t;
}