int Item_copy_string::save_in_field(Field *field, bool no_conversions)
{
  return save_str_value_in_field(field, &str_value);
}