double Item_sp_variable::val_real()
{
  DBUG_ASSERT(fixed);
  Item *it= this_item();
  double ret= it->val_real();
  null_value= it->null_value;
  return ret;
}