add_bitset(regex_t* reg, BitSetRef bs)
{
  BB_ADD(reg, bs, SIZE_BITSET);
  return 0;
}