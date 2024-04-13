add_mode(regex_t* reg, ModeType mode)
{
  BB_ADD(reg, &mode, SIZE_MODE);
  return 0;
}