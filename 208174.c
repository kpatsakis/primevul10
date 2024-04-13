get_millisecs_since (void)
{
  static long last = 0;
  long now = get_millisecs_now();
  long diff = now - last;
  last = now;
  return diff;
}