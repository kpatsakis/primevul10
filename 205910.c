void Item_cache_row::bring_value()
{
  if (!example)
    return;
  example->bring_value();
  null_value= example->null_value;
  for (uint i= 0; i < item_count; i++)
    values[i]->bring_value();
}