double_from_string_with_check(CHARSET_INFO *cs, const char *cptr,
                              const char *end)
{
  int error;
  char *end_of_num= (char*) end;
  double tmp;

  tmp= my_strntod(cs, (char*) cptr, end - cptr, &end_of_num, &error);
  if (error || (end != end_of_num &&
                !check_if_only_end_space(cs, end_of_num, end)))
  {
    ErrConvString err(cptr, end - cptr, cs);
    /*
      We can use err.ptr() here as ErrConvString is guranteed to put an
      end \0 here.
    */
    push_warning_printf(current_thd, MYSQL_ERROR::WARN_LEVEL_WARN,
                        ER_TRUNCATED_WRONG_VALUE,
                        ER(ER_TRUNCATED_WRONG_VALUE), "DOUBLE",
                        err.ptr());
  }
  return tmp;
}