add_length(regex_t* reg, int len)
{
  LengthType l = (LengthType )len;

  BB_ADD(reg, &l, SIZE_LENGTH);
  return 0;
}