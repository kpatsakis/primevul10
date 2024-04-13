int Item_decimal::save_in_field(Field *field, bool no_conversions)
{
  field->set_notnull();
  return field->store_decimal(&decimal_value);
}