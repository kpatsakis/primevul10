int Item_cache_str::save_in_field(Field *field, bool no_conversions)
{
  if (!has_value())
    return set_field_to_null_with_conversions(field, no_conversions);
  int res= Item_cache::save_in_field(field, no_conversions);
  return (is_varbinary && field->type() == MYSQL_TYPE_STRING &&
          value->length() < field->field_length) ? 1 : res;
}