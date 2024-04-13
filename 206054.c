bool Item_cache_row::setup(Item * item)
{
  example= item;
  if (!values && allocate(item->cols()))
    return 1;
  for (uint i= 0; i < item_count; i++)
  {
    Item *el= item->element_index(i);
    Item_cache *tmp;
    if (!(tmp= values[i]= Item_cache::get_cache(el)))
      return 1;
    tmp->setup(el);
  }
  return 0;
}