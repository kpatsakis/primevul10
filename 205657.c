void Item_hex_string::print(String *str, enum_query_type query_type)
{
  str->append("X'");
  str->append_hex(str_value.ptr(), str_value.length());
  str->append("'");
}