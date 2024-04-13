longlong Item_datetime::val_int()
{
  return TIME_to_ulonglong(&ltime);
}