int get_sw_value(buf_T *buf)
{
  long result = buf->b_p_sw ? buf->b_p_sw : buf->b_p_ts;
  assert(result >= 0 && result <= INT_MAX);
  return (int)result;
}