int get_sts_value(void)
{
  long result = curbuf->b_p_sts < 0 ? get_sw_value(curbuf) : curbuf->b_p_sts;
  assert(result >= 0 && result <= INT_MAX);
  return (int)result;
}