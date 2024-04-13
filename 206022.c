bool Item_cache_real::cache_value()
{
  if (!example)
    return FALSE;
  value_cached= TRUE;
  value= example->val_result();
  null_value= example->null_value;
  return TRUE;
}