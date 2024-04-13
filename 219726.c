_pam_parse (pam_handle_t *pamh, int argc, const char **argv,
	    const char **database, const char **cryptmode)
{
  int ctrl;

  *database = NULL;
  *cryptmode = NULL;

  /* step through arguments */
  for (ctrl = 0; argc-- > 0; ++argv)
    {
      /* generic options */

      if (!strcmp(*argv,"debug"))
	ctrl |= PAM_DEBUG_ARG;
      else if (!strcasecmp(*argv, "icase"))
	ctrl |= PAM_ICASE_ARG;
      else if (!strcasecmp(*argv, "dump"))
	ctrl |= PAM_DUMP_ARG;
      else if (!strcasecmp(*argv, "unknown_ok"))
	ctrl |= PAM_UNKNOWN_OK_ARG;
      else if (!strcasecmp(*argv, "key_only"))
	ctrl |= PAM_KEY_ONLY_ARG;
      else if (!strcasecmp(*argv, "use_first_pass"))
	ctrl |= PAM_USE_FPASS_ARG;
      else if (!strcasecmp(*argv, "try_first_pass"))
	ctrl |= PAM_TRY_FPASS_ARG;
      else if (!strncasecmp(*argv,"db=", 3))
	{
	  *database = (*argv) + 3;
	  if (**database == '\0') {
	    *database = NULL;
	    pam_syslog(pamh, LOG_ERR,
		       "db= specification missing argument - ignored");
	  }
	}
      else if (!strncasecmp(*argv,"crypt=", 6))
	{
	  *cryptmode = (*argv) + 6;
	  if (**cryptmode == '\0')
	    pam_syslog(pamh, LOG_ERR,
		       "crypt= specification missing argument - ignored");
	}
      else
	{
	  pam_syslog(pamh, LOG_ERR, "unknown option: %s", *argv);
	}
    }

  return ctrl;
}