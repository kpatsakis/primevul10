int stored_field_cmp_to_item(THD *thd, Field *field, Item *item)
{
  Item_result res_type=item_cmp_type(field->result_type(),
				     item->result_type());
  if (res_type == STRING_RESULT)
  {
    char item_buff[MAX_FIELD_WIDTH];
    char field_buff[MAX_FIELD_WIDTH];
    
    String item_tmp(item_buff,sizeof(item_buff),&my_charset_bin);
    String field_tmp(field_buff,sizeof(field_buff),&my_charset_bin);
    String *item_result= item->val_str(&item_tmp);
    /*
      Some implementations of Item::val_str(String*) actually modify
      the field Item::null_value, hence we can't check it earlier.
    */
    if (item->null_value)
      return 0;
    String *field_result= field->val_str(&field_tmp);

    enum_field_types field_type= field->type();

    if (field_type == MYSQL_TYPE_DATE || field_type == MYSQL_TYPE_DATETIME ||
        field_type == MYSQL_TYPE_TIMESTAMP)
    {
      enum_mysql_timestamp_type type= MYSQL_TIMESTAMP_ERROR;

      if (field_type == MYSQL_TYPE_DATE)
        type= MYSQL_TIMESTAMP_DATE;
      else
        type= MYSQL_TIMESTAMP_DATETIME;
        
      const char *field_name= field->field_name;
      MYSQL_TIME field_time, item_time;
      get_mysql_time_from_str(thd, field_result, type, field_name, &field_time);
      get_mysql_time_from_str(thd, item_result, type, field_name,  &item_time);

      return my_time_compare(&field_time, &item_time);
    }
    return sortcmp(field_result, item_result, field->charset());
  }
  if (res_type == INT_RESULT)
    return 0;					// Both are of type int
  if (res_type == DECIMAL_RESULT)
  {
    my_decimal item_buf, *item_val,
               field_buf, *field_val;
    item_val= item->val_decimal(&item_buf);
    if (item->null_value)
      return 0;
    field_val= field->val_decimal(&field_buf);
    return my_decimal_cmp(field_val, item_val);
  }
  /*
    We have to check field->cmp_type() instead of res_type,
    as result_type() - and thus res_type - can never be TIME_RESULT (yet).
  */
  if (field->cmp_type() == TIME_RESULT)
  {
    MYSQL_TIME field_time, item_time;
    if (field->type() == MYSQL_TYPE_TIME)
    {
      field->get_time(&field_time);
      item->get_time(&item_time);
    }
    else
    {
      field->get_date(&field_time, TIME_INVALID_DATES);
      item->get_date(&item_time, TIME_INVALID_DATES);
    }
    return my_time_compare(&field_time, &item_time);
  }
  /*
    The patch for Bug#13463415 started using this function for comparing
    BIGINTs. That uncovered a bug in Visual Studio 32bit optimized mode.
    Prefixing the auto variables with volatile fixes the problem....
  */
  volatile double result= item->val_real();
  if (item->null_value)
    return 0;
  volatile double field_result= field->val_real();
  if (field_result < result)
    return -1;
  else if (field_result > result)
    return 1;
  return 0;
}