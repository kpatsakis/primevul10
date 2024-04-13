ts_is_unique_and_not_stale(struct timeval *ts, struct timeval *now)
{
  int within_margin=0;
  int is_unique=0;
  long diff;

  diff = now->tv_sec - ts->tv_sec;
  if ((diff < TS_MARGIN) && (diff > -TS_MARGIN)) {
    within_margin = 1;
  } else {
    within_margin = 0;
  }
  is_unique = check_unique_ts(ts, now);
    
  return within_margin && is_unique;
}