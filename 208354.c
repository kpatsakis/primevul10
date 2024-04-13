static int _pam_parse(int argc, CONST char **argv, radius_conf_t *conf)
{
	int ctrl=0;

	memset(conf, 0, sizeof(radius_conf_t)); /* ensure it's initialized */

	strcpy(conf_file, CONF_FILE);

	/* set the default prompt */
	snprintf(conf->prompt, MAXPROMPT, "%s: ", DEFAULT_PROMPT);

	/*
	 *	If either is not there, then we can't parse anything.
	 */
	if ((argc == 0) || (argv == NULL)) {
		return ctrl;
	}

	/* step through arguments */
	for (ctrl=0; argc-- > 0; ++argv) {

		/* generic options */
		if (!strncmp(*argv,"conf=",5)) {
			/* protect against buffer overflow */
			if (strlen(*argv+5) >= sizeof(conf_file)) {
				_pam_log(LOG_ERR, "conf= argument too long");
				conf_file[0] = 0;
				return 0;
			}
			strcpy(conf_file,*argv+5);

		} else if (!strcmp(*argv, "use_first_pass")) {
			ctrl |= PAM_USE_FIRST_PASS;

		} else if (!strcmp(*argv, "try_first_pass")) {
			ctrl |= PAM_TRY_FIRST_PASS;

		} else if (!strcmp(*argv, "skip_passwd")) {
			ctrl |= PAM_SKIP_PASSWD;

		} else if (!strncmp(*argv, "retry=", 6)) {
			conf->retries = atoi(*argv+6);

		} else if (!strcmp(*argv, "localifdown")) {
			conf->localifdown = 1;

		} else if (!strncmp(*argv, "client_id=", 10)) {
			if (conf->client_id) {
				_pam_log(LOG_WARNING, "ignoring duplicate '%s'", *argv);
			} else {
				conf->client_id = (char *) *argv+10; /* point to the client-id */
			}
		} else if (!strcmp(*argv, "accounting_bug")) {
			conf->accounting_bug = TRUE;

		} else if (!strcmp(*argv, "ruser")) {
			ctrl |= PAM_RUSER_ARG;

		} else if (!strcmp(*argv, "debug")) {
			ctrl |= PAM_DEBUG_ARG;
			conf->debug = 1;
			opt_debug = TRUE;

		} else if (!strncmp(*argv, "prompt=", 7)) {
			if (!strncmp(conf->prompt, (char*)*argv+7, MAXPROMPT)) {
				_pam_log(LOG_WARNING, "ignoring duplicate '%s'", *argv);
			} else {
				/* truncate excessive prompts to (MAXPROMPT - 3) length */
				if (strlen((char*)*argv+7) >= (MAXPROMPT - 3)) {
					*((char*)*argv+7 + (MAXPROMPT - 3)) = 0;
				}
				/* set the new prompt */
				memset(conf->prompt, 0, sizeof(conf->prompt));
				snprintf(conf->prompt, MAXPROMPT, "%s: ", (char*)*argv+7);
			}

		} else if (!strcmp(*argv, "force_prompt")) {
			conf->force_prompt= TRUE;

		} else if (!strncmp(*argv, "max_challenge=", 14)) {
			conf->max_challenge = atoi(*argv+14);

		} else {
			_pam_log(LOG_WARNING, "unrecognized option '%s'", *argv);
		}
	}

	return ctrl;
}