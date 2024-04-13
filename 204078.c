retr_rate (wgint bytes, double secs)
{
  static char res[20];
  static const char *rate_names[] = {"B/s", "KB/s", "MB/s", "GB/s" };
  static const char *rate_names_bits[] = {"b/s", "Kb/s", "Mb/s", "Gb/s" };
  int units;

  double dlrate = calc_rate (bytes, secs, &units);
  /* Use more digits for smaller numbers (regardless of unit used),
     e.g. "1022", "247", "12.5", "2.38".  */
  snprintf (res, sizeof(res), "%.*f %s",
           dlrate >= 99.95 ? 0 : dlrate >= 9.995 ? 1 : 2,
           dlrate, !opt.report_bps ? rate_names[units]: rate_names_bits[units]);

  return res;
}