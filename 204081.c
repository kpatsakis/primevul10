calc_rate (wgint bytes, double secs, int *units)
{
  double dlrate;
  double bibyte = 1000.0;

  if (!opt.report_bps)
    bibyte = 1024.0;


  assert (secs >= 0);
  assert (bytes >= 0);

  if (secs == 0)
    /* If elapsed time is exactly zero, it means we're under the
       resolution of the timer.  This can easily happen on systems
       that use time() for the timer.  Since the interval lies between
       0 and the timer's resolution, assume half the resolution.  */
    secs = ptimer_resolution () / 2.0;

  dlrate = convert_to_bits (bytes) / secs;
  if (dlrate < bibyte)
    *units = 0;
  else if (dlrate < (bibyte * bibyte))
    *units = 1, dlrate /= bibyte;
  else if (dlrate < (bibyte * bibyte * bibyte))
    *units = 2, dlrate /= (bibyte * bibyte);

  else
    /* Maybe someone will need this, one day. */
    *units = 3, dlrate /= (bibyte * bibyte * bibyte);

  return dlrate;
}