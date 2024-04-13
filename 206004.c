inline void Item_cache_wrapper::cache()
{
  expr_value->store(orig_item);
  expr_value->cache_value();
  expr_cache->put_value(expr_value); // put in expr_cache
}