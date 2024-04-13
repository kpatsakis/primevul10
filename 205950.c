void Item::print_item_w_name(String *str, enum_query_type query_type)
{
  print(str, query_type);

  if (name)
  {
    THD *thd= current_thd;
    str->append(STRING_WITH_LEN(" AS "));
    append_identifier(thd, str, name, (uint) strlen(name));
  }
}