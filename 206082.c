my_decimal *Item::val_decimal_from_time(my_decimal *decimal_value)
{
  DBUG_ASSERT(fixed == 1);
  MYSQL_TIME ltime;
  if (get_time(&ltime))
  {
    my_decimal_set_zero(decimal_value);
    return 0;
  }
  return date2my_decimal(&ltime, decimal_value);
}