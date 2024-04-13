int Item_datetime::save_in_field(Field *field, bool no_conversions)
{
  field->set_notnull();
  return field->store_time_dec(&ltime, decimals);
}