String *Item_copy_decimal::val_str(String *result)
{
  if (null_value)
    return (String *) 0;
  result->set_charset(&my_charset_bin);
  my_decimal2string(E_DEC_FATAL_ERROR, &cached_value, 0, 0, 0, result);
  return result;
}