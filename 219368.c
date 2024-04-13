string_append(uschar *string, int *size, int *ptr, int count, ...)
{
va_list ap;
int i;

va_start(ap, count);
for (i = 0; i < count; i++)
  {
  uschar *t = va_arg(ap, uschar *);
  string = string_cat(string, size, ptr, t, Ustrlen(t));
  }
va_end(ap);

return string;
}