my_decimal *Item_ref_null_helper::val_decimal(my_decimal *decimal_value)
{
  DBUG_ASSERT(fixed == 1);
  my_decimal *val= (*ref)->val_decimal_result(decimal_value);
  owner->was_null|= null_value= (*ref)->null_value;
  return val;
}