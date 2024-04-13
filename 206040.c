my_decimal *Item_cache_int::val_decimal(my_decimal *decimal_val)
{
  DBUG_ASSERT(fixed == 1);
  if (!has_value())
    return NULL;
  int2my_decimal(E_DEC_FATAL_ERROR, value, unsigned_flag, decimal_val);
  return decimal_val;
}