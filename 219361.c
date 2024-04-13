string_split_message(uschar *msg)
{
uschar *s, *ss;

if (msg == NULL || Ustrlen(msg) <= 75) return msg;
s = ss = msg = string_copy(msg);

for (;;)
  {
  int i = 0;
  while (i < 75 && *ss != 0 && *ss != '\n') ss++, i++;
  if (*ss == 0) break;
  if (*ss == '\n')
    s = ++ss;
  else
    {
    uschar *t = ss + 1;
    uschar *tt = NULL;
    while (--t > s + 35)
      {
      if (*t == ' ')
        {
        if (t[-1] == ':') { tt = t; break; }
        if (tt == NULL) tt = t;
        }
      }

    if (tt == NULL)          /* Can't split behind - try ahead */
      {
      t = ss + 1;
      while (*t != 0)
        {
        if (*t == ' ' || *t == '\n')
          { tt = t; break; }
        t++;
        }
      }

    if (tt == NULL) break;   /* Can't find anywhere to split */
    *tt = '\n';
    s = ss = tt+1;
    }
  }

return msg;
}