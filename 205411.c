void report_or_die(const char *fmt, ...)
{
  va_list args;
  DBUG_ENTER("report_or_die");

  char buff[DIE_BUFF_SIZE];

  va_start(args, fmt);
  make_error_message(buff, sizeof(buff), fmt, args);
  va_end(args);

  if (opt_continue_on_error)
  {
    /* Just log the error and continue */
    replace_dynstr_append(&ds_res, buff);
    error_count++;
    DBUG_VOID_RETURN;
  }

  really_die(buff);
}