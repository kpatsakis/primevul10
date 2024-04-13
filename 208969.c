void eprintf(const char *format, ...) {
   va_list ap;
   char *msg;

   va_start(ap, format);
   xvasprintf(&msg, format, ap);
   va_end(ap);

   (*printf_handler)(msg);

   free(msg);
}