add_option(regex_t* reg, OnigOptionType option)
{
  BB_ADD(reg, &option, SIZE_OPTION);
  return 0;
}