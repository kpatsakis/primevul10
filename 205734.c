void Item_hex_hybrid::print(String *str, enum_query_type query_type)
{
  uint32 len= min(str_value.length(), sizeof(longlong));
  const char *ptr= str_value.ptr() + str_value.length() - len;
  str->append("0x");
  str->append_hex(ptr, len);
}