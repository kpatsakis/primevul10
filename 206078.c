void Item_cache::print(String *str, enum_query_type query_type)
{
  if (value_cached)
  {
    print_value(str);
    return;
  }
  str->append(STRING_WITH_LEN("<cache>("));
  if (example)
    example->print(str, query_type);
  else
    Item::print(str, query_type);
  str->append(')');
}