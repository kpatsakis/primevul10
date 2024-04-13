bool Item_ref::val_bool()
{
  DBUG_ASSERT(fixed);
  bool tmp= (*ref)->val_bool_result();
  null_value= (*ref)->null_value;
  return tmp;
}