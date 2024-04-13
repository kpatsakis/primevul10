tv_to_cycles(const struct timeval& tv, int64_t MHz)
{
  return (((uint64_t)tv.tv_sec * 1000000) + tv.tv_usec) * MHz;
}