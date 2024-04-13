double Item_cache_decimal::val_real()
{
  DBUG_ASSERT(fixed);
  double res;
  if (!has_value())
    return 0.0;
  my_decimal2double(E_DEC_FATAL_ERROR, &decimal_value, &res);
  return res;
}