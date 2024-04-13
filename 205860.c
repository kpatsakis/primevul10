String *Item_copy_uint::val_str(String *str)
{
  if (null_value)
    return (String *) 0;

  str->set((ulonglong) cached_value, &my_charset_bin);
  return str;
}