longlong Item_string::val_int()
{
  DBUG_ASSERT(fixed == 1);
  return longlong_from_string_with_check(str_value.charset(), str_value.ptr(),
                                         str_value.ptr()+ str_value.length());
}