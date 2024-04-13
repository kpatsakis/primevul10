String *Item_copy_float::val_str(String *str)
{
  if (null_value)
    return (String *) 0;
  else
  {
    double nr= val_real();
    str->set_real(nr,decimals, &my_charset_bin);
    return str;
  }
}