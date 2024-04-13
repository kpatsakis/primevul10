int Item_cache_temporal::save_in_field(Field *field, bool no_conversions)
{
  MYSQL_TIME ltime;
  if (get_date(&ltime, 0))
    return set_field_to_null_with_conversions(field, no_conversions);
  field->set_notnull();
  int error= field->store_time_dec(&ltime, decimals);
  return error ? error : field->table->in_use->is_error() ? 1 : 0;
}