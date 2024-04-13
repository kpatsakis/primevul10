void Item_cache_row::store(Item * item)
{
  example= item;
  if (!item)
  {
    null_value= TRUE;
    return;
  }
  for (uint i= 0; i < item_count; i++)
    values[i]->store(item->element_index(i));
}