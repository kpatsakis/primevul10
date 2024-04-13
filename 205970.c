void Item_cache_temporal::store_packed(longlong val_arg, Item *example)
{
  /* An explicit values is given, save it. */
  store(example);
  value_cached= true;
  value= val_arg;
  null_value= false;
}