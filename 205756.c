double Item_copy_string::val_real()
{
  int err_not_used;
  char *end_not_used;
  return (null_value ? 0.0 :
          my_strntod(str_value.charset(), (char*) str_value.ptr(),
                     str_value.length(), &end_not_used, &err_not_used));
}