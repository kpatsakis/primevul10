void Item_datetime::set(longlong packed)
{
  unpack_time(packed, &ltime);
}