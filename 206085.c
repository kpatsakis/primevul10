longlong Item_ref::val_int()
{
  DBUG_ASSERT(fixed);
  longlong tmp=(*ref)->val_int_result();
  null_value=(*ref)->null_value;
  return tmp;
}