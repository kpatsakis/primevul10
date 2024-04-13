String *Item::check_well_formed_result(String *str, bool send_error)
{
  /* Check whether we got a well-formed string */
  CHARSET_INFO *cs= str->charset();
  int well_formed_error;
  uint wlen= cs->cset->well_formed_len(cs,
                                       str->ptr(), str->ptr() + str->length(),
                                       str->length(), &well_formed_error);
  null_value= false;
  if (wlen < str->length())
  {
    THD *thd= current_thd;
    char hexbuf[7];
    uint diff= str->length() - wlen;
    set_if_smaller(diff, 3);
    octet2hex(hexbuf, str->ptr() + wlen, diff);
    if (send_error)
    {
      my_error(ER_INVALID_CHARACTER_STRING, MYF(0),
               cs->csname,  hexbuf);
      return 0;
    }
    if ((thd->variables.sql_mode &
         (MODE_STRICT_TRANS_TABLES | MODE_STRICT_ALL_TABLES)))
    {
      null_value= 1;
      str= 0;
    }
    else
    {
      str->length(wlen);
    }
    push_warning_printf(thd, MYSQL_ERROR::WARN_LEVEL_WARN, ER_INVALID_CHARACTER_STRING,
                        ER(ER_INVALID_CHARACTER_STRING), cs->csname, hexbuf);
  }
  return str;
}