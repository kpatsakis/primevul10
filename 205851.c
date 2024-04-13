longlong Item_direct_ref::val_int()
{
  longlong tmp=(*ref)->val_int();
  null_value=(*ref)->null_value;
  return tmp;
}