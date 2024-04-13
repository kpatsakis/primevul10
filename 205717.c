double Item_string::val_real()
{
  DBUG_ASSERT(fixed == 1);
  return double_from_string_with_check(str_value.charset(),
                                       str_value.ptr(), 
                                       str_value.ptr() +
                                       str_value.length());
}