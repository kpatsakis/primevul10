int Item_cache_wrapper::save_in_field(Field *to, bool no_conversions)
{
  int res;
  DBUG_ASSERT(!result_field);
  res= orig_item->save_in_field(to, no_conversions);
  null_value= orig_item->null_value;
  return res;
}