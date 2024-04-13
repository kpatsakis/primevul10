int Item_float::save_in_field(Field *field, bool no_conversions)
{
  double nr= val_real();
  if (null_value)
    return set_field_to_null(field);
  field->set_notnull();
  return field->store(nr);
}