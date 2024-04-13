my_decimal *Item_copy_float::val_decimal(my_decimal *decimal_value)
{
  if (null_value)
    return (my_decimal *) 0;
  else
  {
    double nr= val_real();
    double2my_decimal(E_DEC_FATAL_ERROR, nr, decimal_value);
    return decimal_value;
  }
}