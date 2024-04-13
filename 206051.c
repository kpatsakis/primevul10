longlong Item::val_int_from_date()
{
  DBUG_ASSERT(fixed == 1);
  MYSQL_TIME ltime;
  if (get_date(&ltime, 0))
    return 0;
  longlong v= TIME_to_ulonglong(&ltime);
  return ltime.neg ? -v : v;
}