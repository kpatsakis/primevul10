string_format(uschar *buffer, int buflen, char *format, ...)
{
BOOL yield;
va_list ap;
va_start(ap, format);
yield = string_vformat(buffer, buflen, format, ap);
va_end(ap);
return yield;
}