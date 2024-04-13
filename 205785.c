double Item_name_const::val_real()
{
  DBUG_ASSERT(fixed);
  double ret= value_item->val_real();
  null_value= value_item->null_value;
  return ret;
}