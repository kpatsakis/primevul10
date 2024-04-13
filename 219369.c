string_copyn(uschar *s, int n)
{
uschar *ss = store_get(n + 1);
Ustrncpy(ss, s, n);
ss[n] = 0;
return ss;
}