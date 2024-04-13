longlong Item_sp_variable::val_int()
{
  DBUG_ASSERT(fixed);
  Item *it= this_item();
  longlong ret= it->val_int();
  null_value= it->null_value;
  return ret;
}