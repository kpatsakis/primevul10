longlong Item_cache_temporal::val_int()
{
  DBUG_ASSERT(fixed == 1);
  if ((!value_cached && !cache_value()) || null_value)
  {
    null_value= true;
    return 0;
  }
  return val_int_from_date();
}