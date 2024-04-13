string_copynlc(uschar *s, int n)
{
uschar *ss = store_get(n + 1);
uschar *p = ss;
while (n-- > 0) *p++ = tolower(*s++);
*p = 0;
return ss;
}