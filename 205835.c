my_decimal *Item_cache_decimal::val_decimal(my_decimal *val)
{
  DBUG_ASSERT(fixed);
  if (!has_value())
    return NULL;
  return &decimal_value;
}