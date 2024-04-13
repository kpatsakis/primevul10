void Item_cache_wrapper::save_val(Field *to)
{
  Item *cached_value;
  DBUG_ENTER("Item_cache_wrapper::val_int");
  if (!expr_cache)
  {
    orig_item->save_val(to);
    null_value= orig_item->null_value;
    DBUG_VOID_RETURN;
  }

  if ((cached_value= check_cache()))
  {
    cached_value->save_val(to);
    null_value= cached_value->null_value;
    DBUG_VOID_RETURN;
  }
  cache();
  null_value= expr_value->null_value;
  expr_value->save_val(to);
  DBUG_VOID_RETURN;
}