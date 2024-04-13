Item_uint::Item_uint(const char *str_arg, longlong i, uint length):
  Item_int(str_arg, i, length)
{
  unsigned_flag= 1;
}