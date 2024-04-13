string_copy_dnsdomain(uschar *s)
{
uschar *yield;
uschar *ss = yield = store_get(Ustrlen(s) + 1);

while (*s != 0)
  {
  if (*s != '\\')
    {
    *ss++ = *s++;
    }
  else if (isdigit(s[1]))
    {
    *ss++ = (s[1] - '0')*100 + (s[2] - '0')*10 + s[3] - '0';
    s += 4;
    }
  else if (*(++s) != 0)
    {
    *ss++ = *s++;
    }
  }

*ss = 0;
return yield;
}