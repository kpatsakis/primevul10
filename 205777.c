double Item::val_real_from_date()
{
  DBUG_ASSERT(fixed == 1);
  MYSQL_TIME ltime;
  if (get_date(&ltime, 0))
    return 0;
  return TIME_to_double(&ltime);
}