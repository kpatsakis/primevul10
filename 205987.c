int Item::save_date_in_field(Field *field)
{
  MYSQL_TIME ltime;
  if (get_date(&ltime, (current_thd->variables.sql_mode &
                        (MODE_NO_ZERO_IN_DATE | MODE_NO_ZERO_DATE |
                         MODE_INVALID_DATES))))
    return set_field_to_null_with_conversions(field, 0);
  field->set_notnull();
  return field->store_time_dec(&ltime, decimals);
}