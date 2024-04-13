bool Item::get_seconds(ulonglong *sec, ulong *sec_part)
{
  if (decimals == 0)
  { // optimize for an important special case
    longlong val= val_int();
    bool neg= val < 0 && !unsigned_flag;
    *sec= neg ? -val : val;
    *sec_part= 0;
    return neg;
  }
  my_decimal tmp, *dec= val_decimal(&tmp);
  if (!dec)
    return 0;
  return my_decimal2seconds(dec, sec, sec_part);
}