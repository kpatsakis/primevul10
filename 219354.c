string_get_localpart(address_item *addr, uschar *yield, int *sizeptr,
  int *ptrptr)
{
if (testflag(addr, af_include_affixes) && addr->prefix != NULL)
  yield = string_cat(yield, sizeptr, ptrptr, addr->prefix,
    Ustrlen(addr->prefix));
yield = string_cat(yield, sizeptr, ptrptr, addr->local_part,
  Ustrlen(addr->local_part));
if (testflag(addr, af_include_affixes) && addr->suffix != NULL)
  yield = string_cat(yield, sizeptr, ptrptr, addr->suffix,
    Ustrlen(addr->suffix));
return yield;
}