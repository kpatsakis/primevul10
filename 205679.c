Item* Item_cache_wrapper::get_tmp_table_item(THD *thd_arg)
{
  if (!orig_item->with_sum_func && !orig_item->const_item())
    return new Item_field(result_field);
  return copy_or_same(thd_arg);
}