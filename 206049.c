bool Item_ref_null_helper::val_bool()
{
  DBUG_ASSERT(fixed == 1);
  bool val= (*ref)->val_bool_result();
  owner->was_null|= null_value= (*ref)->null_value;
  return val;
}