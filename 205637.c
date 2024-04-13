longlong Item_field::val_int_endpoint(bool left_endp, bool *incl_endp)
{
  longlong res= val_int();
  return null_value? LONGLONG_MIN : res;
}