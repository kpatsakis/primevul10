bool Item_cache_row::cache_value()
{
  if (!example)
    return FALSE;
  value_cached= TRUE;
  null_value= 0;
  example->bring_value();
  for (uint i= 0; i < item_count; i++)
  {
    values[i]->cache_value();
    null_value|= values[i]->null_value;
  }
  return TRUE;
}