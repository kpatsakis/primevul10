longlong Item_name_const::val_int()
{
  DBUG_ASSERT(fixed);
  longlong ret= value_item->val_int();
  null_value= value_item->null_value;
  return ret;
}