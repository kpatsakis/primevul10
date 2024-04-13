longlong Item_cache_temporal::val_temporal_packed()
{
  DBUG_ASSERT(fixed == 1);
  if ((!value_cached && !cache_value()) || null_value)
  {
    null_value= TRUE;
    return 0;
  }
  return value;
}