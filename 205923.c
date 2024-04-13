void Item_cache::store(Item *item)
{
  example= item;
  if (!item)
    null_value= TRUE;
  value_cached= FALSE;
}