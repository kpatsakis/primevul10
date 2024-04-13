void Item_cache_row::set_null()
{
  Item_cache::set_null();
  if (!values)
    return;
  for (uint i= 0; i < item_count; i++)
    values[i]->set_null();
};