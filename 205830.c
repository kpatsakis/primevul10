int Item::save_time_in_field(Field *field)
{
  MYSQL_TIME ltime;
  if (get_time(&ltime))
    return set_field_to_null_with_conversions(field, 0);
  field->set_notnull();
  return field->store_time_dec(&ltime, decimals);
}