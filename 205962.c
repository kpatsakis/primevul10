String *Item_copy_int::val_str(String *str)
{
  if (null_value)
    return (String *) 0;

  str->set(cached_value, &my_charset_bin);
  return str;
}