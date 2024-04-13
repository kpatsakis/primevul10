String *Item::val_string_from_date(String *str)
{
  MYSQL_TIME ltime;
  if (get_date(&ltime,
               field_type() == MYSQL_TYPE_TIME ? TIME_TIME_ONLY : 0) ||
      str->alloc(MAX_DATE_STRING_REP_LENGTH))
  {
    null_value= 1;
    return (String *) 0;
  }
  str->length(my_TIME_to_str(&ltime, const_cast<char*>(str->ptr()), decimals));
  str->set_charset(&my_charset_numeric);
  return str;
}