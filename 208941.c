size_t my_strftime(char *s, size_t max, const char *fmt, const struct tm *tm) {
   return strftime(s, max, fmt, tm);
}