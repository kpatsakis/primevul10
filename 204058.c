time_to_rfc1123 (time_t time, char *buf, size_t bufsize)
{
  static const char *wkday[] = { "Sun", "Mon", "Tue", "Wed",
                                 "Thu", "Fri", "Sat" };
  static const char *month[] = { "Jan", "Feb", "Mar", "Apr",
                                 "May", "Jun", "Jul", "Aug",
                                 "Sep", "Oct", "Nov", "Dec" };
  /* rfc1123 example: Thu, 01 Jan 1998 22:12:57 GMT  */
  static const char *time_format = "%s, %02d %s %04d %02d:%02d:%02d GMT";

  struct tm *gtm = gmtime (&time);
  if (!gtm)
    {
      logprintf (LOG_NOTQUIET,
                 _("gmtime failed. This is probably a bug.\n"));
      return TIMECONV_ERR;
    }

  snprintf (buf, bufsize, time_format, wkday[gtm->tm_wday],
            gtm->tm_mday, month[gtm->tm_mon],
            gtm->tm_year + 1900, gtm->tm_hour,
            gtm->tm_min, gtm->tm_sec);

  return RETROK;
}