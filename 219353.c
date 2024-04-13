string_copy(uschar *s)
{
int len = Ustrlen(s) + 1;
uschar *ss = store_get(len);
memcpy(ss, s, len);
return ss;
}