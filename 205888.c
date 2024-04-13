String *Item::val_str_ascii(String *str)
{
  DBUG_ASSERT(str != &str_value);
  
  uint errors;
  String *res= val_str(&str_value);
  if (!res)
    return 0;
  
  if (!(res->charset()->state & MY_CS_NONASCII))
    str= res;
  else
  {
    if ((null_value= str->copy(res->ptr(), res->length(), collation.collation,
                               &my_charset_latin1, &errors)))
      return 0;
  }

  return str;
}