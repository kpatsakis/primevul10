String *Item_name_const::val_str(String *sp)
{
  DBUG_ASSERT(fixed);
  String *ret= value_item->val_str(sp);
  null_value= value_item->null_value;
  return ret;
}