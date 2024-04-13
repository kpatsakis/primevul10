bool Item_cache_wrapper::fix_fields(THD *thd  __attribute__((unused)),
                                    Item **it __attribute__((unused)))
{
  DBUG_ASSERT(orig_item->fixed);
  DBUG_ASSERT(fixed);
  return FALSE;
}