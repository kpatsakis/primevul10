uint Item::temporal_precision(enum_field_types type)
{
  if (const_item() && result_type() == STRING_RESULT &&
      !is_temporal_type(field_type()))
  {
    MYSQL_TIME ltime;
    String buf, *tmp;
    int was_cut;
    DBUG_ASSERT(fixed);
    if ((tmp= val_str(&buf)) &&
        (type == MYSQL_TYPE_TIME ?
         str_to_time(tmp->charset(), tmp->ptr(), tmp->length(),
                     &ltime, TIME_TIME_ONLY, &was_cut) :
         str_to_datetime(tmp->charset(), tmp->ptr(), tmp->length(),
                         &ltime, TIME_FUZZY_DATES, &was_cut)) >
        MYSQL_TIMESTAMP_ERROR)
      return min(ms_to_precision(ltime.second_part), TIME_SECOND_PART_DIGITS);
  }
  return min(decimals, TIME_SECOND_PART_DIGITS);
}