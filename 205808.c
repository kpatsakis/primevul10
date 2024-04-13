void Item_trigger_field::print(String *str, enum_query_type query_type)
{
  str->append((row_version == NEW_ROW) ? "NEW" : "OLD", 3);
  str->append('.');
  str->append(field_name);
}