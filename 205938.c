my_decimal *Item_cache_str::val_decimal(my_decimal *decimal_val)
{
  DBUG_ASSERT(fixed == 1);
  if (!has_value())
    return NULL;
  if (value)
    string2my_decimal(E_DEC_FATAL_ERROR, value, decimal_val);
  else
    decimal_val= 0;
  return decimal_val;
}