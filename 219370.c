string_sprintf(char *format, ...)
{
va_list ap;
uschar buffer[STRING_SPRINTF_BUFFER_SIZE];
va_start(ap, format);
if (!string_vformat(buffer, sizeof(buffer), format, ap))
  log_write(0, LOG_MAIN|LOG_PANIC_DIE,
    "string_sprintf expansion was longer than %d", sizeof(buffer));
va_end(ap);
return string_copy(buffer);
}