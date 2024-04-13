my_decimal *Item::val_decimal_from_date(my_decimal *decimal_value)
{
  DBUG_ASSERT(fixed == 1);
  MYSQL_TIME ltime;
  if (get_date(&ltime, 0))
  {
    my_decimal_set_zero(decimal_value);
    null_value= 1;                               // set NULL, stop processing
    return 0;
  }
  return date2my_decimal(&ltime, decimal_value);
}