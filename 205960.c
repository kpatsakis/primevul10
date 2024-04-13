bool Item::get_date(MYSQL_TIME *ltime,ulonglong fuzzydate)
{
  if (field_type() == MYSQL_TYPE_TIME)
    fuzzydate|= TIME_TIME_ONLY;

  switch (result_type()) {
  case INT_RESULT:
  {
    longlong value= val_int();
    bool neg= !unsigned_flag && value < 0;
    if (field_type() == MYSQL_TYPE_YEAR)
    {
      if (max_length == 2)
      {
        if (value < 70)
          value+= 2000;
        else if (value <= 1900)
          value+= 1900;
      }
      value*= 10000; /* make it YYYYMMHH */
    }
    if (null_value || int_to_datetime_with_warn(neg, neg ? -value : value,
                                                ltime, fuzzydate,
                                                field_name_or_null()))
      goto err;
    break;
  }
  case REAL_RESULT:
  {
    double value= val_real();
    if (null_value || double_to_datetime_with_warn(value, ltime, fuzzydate,
                                                   field_name_or_null()))
      goto err;
    break;
  }
  case DECIMAL_RESULT:
  {
    my_decimal value, *res;
    if (!(res= val_decimal(&value)) ||
        decimal_to_datetime_with_warn(res, ltime, fuzzydate,
                                      field_name_or_null()))
      goto err;
    break;
  }
  case STRING_RESULT:
  {
    char buff[40];
    String tmp(buff,sizeof(buff), &my_charset_bin),*res;
    if (!(res=val_str(&tmp)) ||
        str_to_datetime_with_warn(res->charset(), res->ptr(), res->length(),
                                  ltime, fuzzydate) <= MYSQL_TIMESTAMP_ERROR)
      goto err;
    break;
  }
  default:
    DBUG_ASSERT(0);
  }

  return null_value= 0;

err:
  /*
    if the item was not null and convertion failed, we return a zero date
    if allowed, otherwise - null.
  */
  bzero((char*) ltime,sizeof(*ltime));
  return null_value|= !(fuzzydate & TIME_FUZZY_DATES);
}