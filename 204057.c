parse_strict_transport_security (const char *header, time_t *max_age, bool *include_subdomains)
{
  param_token name, value;
  const char *c_max_age = NULL;
  bool is = false; /* includeSubDomains */
  bool is_url_encoded = false;
  bool success = false;

  if (header)
    {
      /* Process the STS header. Keys should be matched case-insensitively. */
      for (; extract_param (&header, &name, &value, ';', &is_url_encoded); is_url_encoded = false)
        {
          if (BOUNDED_EQUAL_NO_CASE (name.b, name.e, "max-age"))
            {
              xfree (c_max_age);
              c_max_age = strdupdelim (value.b, value.e);
            }
          else if (BOUNDED_EQUAL_NO_CASE (name.b, name.e, "includeSubDomains"))
            is = true;
        }

      /* pass the parsed values over */
      if (c_max_age)
        {
          /* If the string value goes out of a long's bounds, strtol() will return LONG_MIN or LONG_MAX.
           * In theory, the HSTS engine should be able to handle it.
           * Also, time_t is normally defined as a long, so this should not break.
           */
          if (max_age)
            *max_age = (time_t) strtol (c_max_age, NULL, 10);
          if (include_subdomains)
            *include_subdomains = is;

          DEBUGP (("Parsed Strict-Transport-Security max-age = %s, includeSubDomains = %s\n",
                 c_max_age, (is ? "true" : "false")));

          xfree (c_max_age);
          success = true;
        }
      else
        {
          /* something weird happened */
          logprintf (LOG_VERBOSE, "Could not parse String-Transport-Security header\n");
          success = false;
        }
    }

  return success;
}