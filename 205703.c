longlong Item_copy_string::val_int()
{
  int err;
  return null_value ? LL(0) : my_strntoll(str_value.charset(),str_value.ptr(),
                                          str_value.length(),10, (char**) 0,
                                          &err); 
}