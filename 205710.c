Item *Item_default_value::transform(Item_transformer transformer, uchar *args)
{
  DBUG_ASSERT(!current_thd->stmt_arena->is_stmt_prepare());

  /*
    If the value of arg is NULL, then this object represents a constant,
    so further transformation is unnecessary (and impossible).
  */
  if (!arg)
    return 0;

  Item *new_item= arg->transform(transformer, args);
  if (!new_item)
    return 0;

  /*
    THD::change_item_tree() should be called only if the tree was
    really transformed, i.e. when a new item has been created.
    Otherwise we'll be allocating a lot of unnecessary memory for
    change records at each execution.
  */
  if (arg != new_item)
    current_thd->change_item_tree(&arg, new_item);
  return (this->*transformer)(args);
}