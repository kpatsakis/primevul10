double Item_cache_str::val_real()
{
  DBUG_ASSERT(fixed == 1);
  int err_not_used;
  char *end_not_used;
  if (!has_value())
    return 0.0;
  if (value)
    return my_strntod(value->charset(), (char*) value->ptr(),
		      value->length(), &end_not_used, &err_not_used);
  return (double) 0;
}