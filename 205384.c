static void make_error_message(char *buf, size_t len, const char *fmt, va_list args)
{
  char *s= buf, *end= buf + len;
  s+= my_snprintf(s, end - s, "mysqltest: ");
  if (cur_file && cur_file != file_stack)
  {
    s+= my_snprintf(s, end - s, "In included file \"%s\": \n",
                    cur_file->file_name);
    s+= print_file_stack(s, end);
  }
  
  if (start_lineno > 0)
    s+= my_snprintf(s, end -s, "At line %u: ", start_lineno);
  if (!fmt)
    fmt= "unknown error";

  s+= my_vsnprintf(s, end - s, fmt, args);
  s+= my_snprintf(s, end -s, "\n", start_lineno);
}