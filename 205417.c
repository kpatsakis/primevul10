ulonglong timer_now(void)
{
  return my_interval_timer() / 1000000;
}