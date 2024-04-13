longlong Item_copy_decimal::val_int()
{
  if (null_value)
    return LL(0);
  else
  {
    longlong result;
    my_decimal2int(E_DEC_FATAL_ERROR, &cached_value, unsigned_flag, &result);
    return result;
  }
}