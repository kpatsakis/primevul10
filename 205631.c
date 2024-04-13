my_decimal *Item_sp_variable::val_decimal(my_decimal *decimal_value)
{
  DBUG_ASSERT(fixed);
  Item *it= this_item();
  my_decimal *val= it->val_decimal(decimal_value);
  null_value= it->null_value;
  return val;
}