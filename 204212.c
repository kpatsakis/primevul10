locale_to_utf8 (const char *str)
{
  char *new;

  /* That shouldn't happen, just in case */
  if (!opt.locale)
    {
      logprintf (LOG_VERBOSE, _("locale_to_utf8: locale is unset\n"));
      opt.locale = find_locale ();
    }

  if (!opt.locale || !c_strcasecmp (opt.locale, "utf-8"))
    return str;

  if (do_conversion ("UTF-8", opt.locale, (char *) str, strlen ((char *) str), &new))
    return (const char *) new;

  xfree (new);
  return str;
}