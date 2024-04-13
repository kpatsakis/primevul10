longlong_from_string_with_check(CHARSET_INFO *cs, const char *cptr,
                                const char *end)
{
  int err;
  longlong tmp;
  char *end_of_num= (char*) end;

  tmp= (*(cs->cset->strtoll10))(cs, cptr, &end_of_num, &err);
  /*
    TODO: Give error if we wanted a signed integer and we got an unsigned
    one
  */
  if (!current_thd->no_errors &&
      (err > 0 ||
       (end != end_of_num && !check_if_only_end_space(cs, end_of_num, end))))
  {
    ErrConvString err(cptr, end - cptr, cs);
    push_warning_printf(current_thd, MYSQL_ERROR::WARN_LEVEL_WARN,
                        ER_TRUNCATED_WRONG_VALUE,
                        ER(ER_TRUNCATED_WRONG_VALUE), "INTEGER",
                        err.ptr());
  }
  return tmp;
}