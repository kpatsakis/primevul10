void Item_cache_wrapper::init_on_demand()
{
    if (!expr_cache->is_inited())
    {
      orig_item->get_cache_parameters(parameters);
      expr_cache->init();
    }
}