my_decimal *Item_name_const::val_decimal(my_decimal *decimal_value)
{
  DBUG_ASSERT(fixed);
  my_decimal *val= value_item->val_decimal(decimal_value);
  null_value= value_item->null_value;
  return val;
}