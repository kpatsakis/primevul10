Item_uint::Item_uint(const char *str_arg, uint length):
  Item_int(str_arg, length)
{
  unsigned_flag= 1;
}