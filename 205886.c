String* Item_cache_str::val_str(String *str)
{
  DBUG_ASSERT(fixed == 1);
  if (!has_value())
    return 0;
  return value;
}