my_decimal *Item_cache_temporal::val_decimal(my_decimal *decimal_value)
{
  DBUG_ASSERT(fixed == 1);
  if ((!value_cached && !cache_value()) || null_value)
  {
    null_value= true;
    return NULL;
  }
  return val_decimal_from_date(decimal_value);
}