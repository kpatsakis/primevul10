Item* Item_ref::transform(Item_transformer transformer, uchar *arg)
{
  DBUG_ASSERT(!current_thd->stmt_arena->is_stmt_prepare());
  DBUG_ASSERT((*ref) != NULL);

  /* Transform the object we are referencing. */
  Item *new_item= (*ref)->transform(transformer, arg);
  if (!new_item)
    return NULL;

  /*
    THD::change_item_tree() should be called only if the tree was
    really transformed, i.e. when a new item has been created.
    Otherwise we'll be allocating a lot of unnecessary memory for
    change records at each execution.
  */
  if (*ref != new_item)
    current_thd->change_item_tree(ref, new_item);

  /* Transform the item ref object. */
  return (this->*transformer)(arg);
}