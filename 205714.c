void Item_cache_wrapper::print(String *str, enum_query_type query_type)
{
  str->append(func_name());
  if (expr_cache)
  {
    init_on_demand();
    expr_cache->print(str, query_type);
  }
  else
    str->append(STRING_WITH_LEN("<<DISABLED>>"));
  str->append('(');
  orig_item->print(str, query_type);
  str->append(')');
}