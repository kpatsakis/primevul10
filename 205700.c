int Item_field::save_in_field(Field *to, bool no_conversions)
{
  return save_field_in_field(result_field, &null_value, to, no_conversions);
}