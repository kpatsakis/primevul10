int Item_null::save_safe_in_field(Field *field)
{
  return set_field_to_null(field);
}