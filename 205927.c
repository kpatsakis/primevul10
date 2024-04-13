double Item_ref::val_real()
{
  DBUG_ASSERT(fixed);
  double tmp=(*ref)->val_result();
  null_value=(*ref)->null_value;
  return tmp;
}