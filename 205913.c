bool Item_field::val_bool_result()
{
  if ((null_value= result_field->is_null()))
    return FALSE;
  switch (result_field->result_type()) {
  case INT_RESULT:
    return result_field->val_int() != 0;
  case DECIMAL_RESULT:
  {
    my_decimal decimal_value;
    my_decimal *val= result_field->val_decimal(&decimal_value);
    if (val)
      return !my_decimal_is_zero(val);
    return 0;
  }
  case REAL_RESULT:
  case STRING_RESULT:
    return result_field->val_real() != 0.0;
  case ROW_RESULT:
  case TIME_RESULT:
  case IMPOSSIBLE_RESULT:
    DBUG_ASSERT(0);
    return 0;                                   // Shut up compiler
  }
  return 0;
}