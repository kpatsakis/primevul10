String* Item_cache_real::val_str(String *str)
{
  DBUG_ASSERT(fixed == 1);
  if (!has_value())
    return NULL;
  str->set_real(value, decimals, default_charset());
  return str;
}