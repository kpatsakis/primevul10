String *Item::val_str(String *str, String *converter, CHARSET_INFO *cs)
{
  String *res= val_str(str);
  if (null_value)
    return (String *) 0;

  if (!cs)
    return res;

  uint errors;
  if ((null_value= converter->copy(res->ptr(), res->length(),
                                   collation.collation, cs,  &errors)))
    return (String *) 0;

  return converter;
}