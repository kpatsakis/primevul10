my_decimal *Item_copy_int::val_decimal(my_decimal *decimal_value)
{
  if (null_value)
    return (my_decimal *) 0;

  int2my_decimal(E_DEC_FATAL_ERROR, cached_value, unsigned_flag, decimal_value);
  return decimal_value;
}