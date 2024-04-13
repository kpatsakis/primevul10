void Item_name_const::print(String *str, enum_query_type query_type)
{
  str->append(STRING_WITH_LEN("NAME_CONST("));
  name_item->print(str, query_type);
  str->append(',');
  value_item->print(str, query_type);
  str->append(')');
}