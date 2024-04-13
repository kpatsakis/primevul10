bool  Item_cache_int::cache_value()
{
  if (!example)
    return FALSE;
  value_cached= TRUE;
  value= example->val_int_result();
  null_value= example->null_value;
  unsigned_flag= example->unsigned_flag;
  return TRUE;
}