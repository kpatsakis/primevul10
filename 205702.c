double Item_cache_int::val_real()
{
  DBUG_ASSERT(fixed == 1);
  if (!has_value())
    return 0.0;
  return (double) value;
}