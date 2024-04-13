double Item_copy_decimal::val_real()
{
  if (null_value)
    return 0.0;
  else
  {
    double result;
    my_decimal2double(E_DEC_FATAL_ERROR, &cached_value, &result);
    return result;
  }
}