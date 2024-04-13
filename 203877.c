do_time_checks(void)
{
  struct timeval now;
  time_t warning_advance = 3600 * 24 * 365 * 10; /* 10 years */

#ifdef HAVE_LONG_TIME_T
  /* Check that time before NTP_ERA_SPLIT underflows correctly */

  struct timeval tv1 = {NTP_ERA_SPLIT, 1}, tv2 = {NTP_ERA_SPLIT - 1, 1};
  NTP_int64 ntv1, ntv2;
  int r;

  UTI_TimevalToInt64(&tv1, &ntv1, 0);
  UTI_TimevalToInt64(&tv2, &ntv2, 0);
  UTI_Int64ToTimeval(&ntv1, &tv1);
  UTI_Int64ToTimeval(&ntv2, &tv2);

  r = tv1.tv_sec == NTP_ERA_SPLIT &&
      tv1.tv_sec + (1ULL << 32) - 1 == tv2.tv_sec;

  assert(r);

  LCL_ReadRawTime(&now);
  if (tv2.tv_sec - now.tv_sec < warning_advance)
    LOG(LOGS_WARN, LOGF_NtpCore, "Assumed NTP time ends at %s!",
        UTI_TimeToLogForm(tv2.tv_sec));
#else
  LCL_ReadRawTime(&now);
  if (now.tv_sec > 0x7fffffff - warning_advance)
    LOG(LOGS_WARN, LOGF_NtpCore, "System time ends at %s!",
        UTI_TimeToLogForm(0x7fffffff));
#endif
}