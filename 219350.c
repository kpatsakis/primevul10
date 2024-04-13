string_copylc(uschar *s)
{
uschar *ss = store_get(Ustrlen(s) + 1);
uschar *p = ss;
while (*s != 0) *p++ = tolower(*s++);
*p = 0;
return ss;
}