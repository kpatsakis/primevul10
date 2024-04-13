String* Item_cache_decimal::val_str(String *str)
{
  DBUG_ASSERT(fixed);
  if (!has_value())
    return NULL;
  my_decimal_round(E_DEC_FATAL_ERROR, &decimal_value, decimals, FALSE,
                   &decimal_value);
  my_decimal2string(E_DEC_FATAL_ERROR, &decimal_value, 0, 0, 0, str);
  return str;
}