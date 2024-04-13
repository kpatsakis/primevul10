void Item_ref_null_helper::print(String *str, enum_query_type query_type)
{
  str->append(STRING_WITH_LEN("<ref_null_helper>("));
  if (ref)
    (*ref)->print(str, query_type);
  else
    str->append('?');
  str->append(')');
}