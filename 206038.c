String *Item_cache_int::val_str(String *str)
{
  DBUG_ASSERT(fixed == 1);
  if (!has_value())
    return NULL;
  str->set_int(value, unsigned_flag, default_charset());
  return str;
}