Item_cache_wrapper::Item_cache_wrapper(Item *item_arg)
:orig_item(item_arg), expr_cache(NULL), expr_value(NULL)
{
  DBUG_ASSERT(orig_item->fixed);
  max_length= orig_item->max_length;
  maybe_null= orig_item->maybe_null;
  decimals=   orig_item->decimals;
  collation.set(orig_item->collation);
  with_sum_func= orig_item->with_sum_func;
  with_field= orig_item->with_field;
  unsigned_flag= orig_item->unsigned_flag;
  name= item_arg->name;
  name_length= item_arg->name_length;
  with_subselect=  orig_item->with_subselect;

  if ((expr_value= Item_cache::get_cache(orig_item)))
    expr_value->setup(orig_item);

  fixed= 1;
}