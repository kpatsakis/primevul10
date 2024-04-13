String *Item_cache_temporal::val_str(String *str)
{
  DBUG_ASSERT(fixed == 1);
  if (!has_value())
  {
    null_value= true;
    return NULL;
  }
  return val_string_from_date(str);
}