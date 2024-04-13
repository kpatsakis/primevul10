longlong Item_cache_real::val_int()
{
  DBUG_ASSERT(fixed == 1);
  if (!has_value())
    return 0;
  return (longlong) rint(value);
}