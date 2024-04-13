void Item_cache::set_null()
{
  if (maybe_null)
  {
    null_value= TRUE;
    value_cached= TRUE;
  }
}