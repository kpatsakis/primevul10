Item_cache* Item_cache::get_cache(const Item *item)
{
  return get_cache(item, item->cmp_type());
}