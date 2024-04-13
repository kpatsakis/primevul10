longlong Item_cache_decimal::val_int()
{
  DBUG_ASSERT(fixed);
  longlong res;
  if (!has_value())
    return 0;
  my_decimal2int(E_DEC_FATAL_ERROR, &decimal_value, unsigned_flag, &res);
  return res;
}