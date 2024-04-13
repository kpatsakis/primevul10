string_copy_malloc(uschar *s)
{
int len = Ustrlen(s) + 1;
uschar *ss = store_malloc(len);
memcpy(ss, s, len);
return ss;
}