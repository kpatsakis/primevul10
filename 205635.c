Item* Item::cache_const_expr_transformer(uchar *arg)
{
  if (*(bool*)arg)
  {
    *((bool*)arg)= FALSE;
    Item_cache *cache= Item_cache::get_cache(this);
    if (!cache)
      return NULL;
    cache->setup(this);
    cache->store(this);
    return cache;
  }
  return this;
}