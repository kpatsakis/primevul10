strcmpic(uschar *s, uschar *t)
{
while (*s != 0)
  {
  int c = tolower(*s++) - tolower(*t++);
  if (c != 0) return c;
  }
return *t;
}