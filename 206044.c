bool Item_cache_decimal::cache_value()
{
  if (!example)
    return FALSE;
  value_cached= TRUE;
  my_decimal *val= example->val_decimal_result(&decimal_value);
  if (!(null_value= example->null_value) && val != &decimal_value)
    my_decimal2decimal(val, &decimal_value);
  return TRUE;
}