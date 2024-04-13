dehyphen (char *s)
{
  char *t = s;                  /* t - tortoise */
  char *h = s;                  /* h - hare     */
  while (*h)
    if (*h == '_' || *h == '-')
      ++h;
    else
      *t++ = *h++;
  *t = '\0';
}