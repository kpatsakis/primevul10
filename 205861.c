void Item::set_name(const char *str, uint length, CHARSET_INFO *cs)
{
  if (!length)
  {
    /* Empty string, used by AS or internal function like last_insert_id() */
    name= (char*) str;
    name_length= 0;
    return;
  }
  if (cs->ctype)
  {
    const char *str_start= str;

    /*
      This will probably need a better implementation in the future:
      a function in CHARSET_INFO structure.
    */
    while (length && !my_isgraph(cs,*str))
    {						// Fix problem with yacc
      length--;
      str++;
    }
    if (str != str_start && !is_autogenerated_name)
    {
      char buff[SAFE_NAME_LEN];
      strmake(buff, str_start,
              min(sizeof(buff)-1, length + (int) (str-str_start)));

      if (length == 0)
        push_warning_printf(current_thd, MYSQL_ERROR::WARN_LEVEL_WARN,
                            ER_NAME_BECOMES_EMPTY, ER(ER_NAME_BECOMES_EMPTY),
                            buff);
      else
        push_warning_printf(current_thd, MYSQL_ERROR::WARN_LEVEL_WARN,
                            ER_REMOVED_SPACES, ER(ER_REMOVED_SPACES),
                            buff);
    }
  }
  if (!my_charset_same(cs, system_charset_info))
  {
    size_t res_length;
    name= sql_strmake_with_convert(str, length, cs,
				   MAX_ALIAS_NAME, system_charset_info,
				   &res_length);
    name_length= res_length;
  }
  else
    name= sql_strmake(str, (name_length= min(length,MAX_ALIAS_NAME)));
}