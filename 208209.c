main_atou (const char* arg, usize_t *uo, usize_t low,
	   usize_t high, char which) 
{
  int ret;
  xoff_t xo;
  if ((ret = main_atoux (arg, &xo, low, high, which)))
    {
      return ret;
    }
  *uo = (usize_t)xo;
  return 0;
}