add_pattern(char *s)
{
	char *first = NULL;
	char *type  = NULL;
	int stored = 0;
	int regex_flags = REG_NOSUB;
	SCPattern currItem;
	char *end = NULL;

	/* skip empty and commented lines */
	if ( (xstrnlen(s, LOW_BUFF) == 0) || (strncmp(s, "#", 1) == 0)) return 1;

	/* Config file directives are construct as follow: name value */  
	type = (char *)malloc(sizeof(char)*LOW_CHAR);
	first = (char *)malloc(sizeof(char)*LOW_BUFF);
	stored = sscanf(s, "%31s %255[^#]", type, first);
  
	if (stored < 2) {
		ci_debug_printf(0, "FATAL add_patterns: Bad configuration line for [%s]\n", s);
		xfree(type);
		xfree(first);
		return 0;
	}
	/* remove extra space or tabulation */
	trim(first);

	/* URl to redirect Squid on virus found */  
	if(strcmp(type, "redirect") == 0) {
		redirect_url = (char *) malloc (sizeof (char) * LOW_BUFF);
		if(redirect_url == NULL) {
			fprintf(stderr, "unable to allocate memory in add_to_patterns()\n");
			xfree(type);
			xfree(first);
			return 0;
		} else {
			xstrncpy(redirect_url, first, LOW_BUFF);
		}
		xfree(type);
		xfree(first);
		return 1;
	}

	/* Path to chained other Squid redirector, mostly SquidGuard */
	if(strcmp(type, "squidguard") == 0) {
		squidguard = (char *) malloc (sizeof (char) * LOW_BUFF);
		if(squidguard == NULL) {
			fprintf(stderr, "unable to allocate memory in add_to_patterns()\n");
			xfree(type);
			xfree(first);
			return 0;
		} else {
			if (isPathExists(first) == 0) {
				xstrncpy(squidguard, first, LOW_BUFF);
			} else {
				ci_debug_printf(0, "LOG add_patterns: Wrong path to SquidGuard, disabling.\n");
				squidguard = NULL;
			}
		}
		xfree(type);
		xfree(first);
		return 1;
	}
  
	if(strcmp(type, "debug") == 0) {
		if (debug == 0)
		   debug = atoi(first);
		xfree(type);
		xfree(first);
		return 1;
	}

	if(strcmp(type, "logredir") == 0) {
		if (logredir == 0)
		   logredir = atoi(first);
		xfree(type);
		xfree(first);
		return 1;
	}

	if(strcmp(type, "dnslookup") == 0) {
		if (dnslookup == 1)
		   dnslookup = atoi(first);
		xfree(type);
		xfree(first);
		return 1;
	}

	if(strcmp(type, "timeout") == 0) {
		timeout = atoi(first);
		if (timeout > 10)
			timeout = 10;
		xfree(type);
		xfree(first);
		return 1;
	}

	if(strcmp(type, "stat") == 0) {
		statit = atoi(first);
		xfree(type);
		xfree(first);
		return 1;
	}

	if(strcmp(type, "clamd_ip") == 0) {
		clamd_ip = (char *) malloc (sizeof (char) * SMALL_CHAR);
		if (clamd_ip == NULL) {
			fprintf(stderr, "unable to allocate memory in add_to_patterns()\n");
			xfree(type);
			xfree(first);
			return 0;
		} else {
			xstrncpy(clamd_ip, first, SMALL_CHAR);
		}
		xfree(type);
		xfree(first);
		return 1;
	}

	if(strcmp(type, "clamd_port") == 0) {
		clamd_port = (char *) malloc (sizeof (char) * LOW_CHAR);
		if(clamd_port == NULL) {
			fprintf(stderr, "unable to allocate memory in add_to_patterns()\n");
			xfree(type);
			xfree(first);
			return 0;
		} else {
			xstrncpy(clamd_port, first, LOW_CHAR);
		}
		xfree(type);
		xfree(first);
		return 1;
	}

	if(strcmp(type, "clamd_local") == 0) {
		clamd_local = (char *) malloc (sizeof (char) * LOW_BUFF);
		if(clamd_local == NULL) {
			fprintf(stderr, "unable to allocate memory in add_to_patterns()\n");
			xfree(type);
			xfree(first);
			return 0;
		} else {
			xstrncpy(clamd_local, first, LOW_BUFF);
		}
		xfree(type);
		xfree(first);
		return 1;
	}

	if (strcmp(type, "maxsize") == 0) {
		maxsize = ci_strto_off_t(first, &end, 10);
		if ((maxsize == 0 && errno != 0) || maxsize < 0)
			maxsize = 0;
		if (*end == 'k' || *end == 'K')
			maxsize = maxsize * 1024;
		else if (*end == 'm' || *end == 'M')
			maxsize = maxsize * 1024 * 1024;
		else if (*end == 'g' || *end == 'G')
			maxsize = maxsize * 1024 * 1024 * 1024;
		xfree(type);
		xfree(first);
		return 1;
	}

	/* force case insensitive pattern matching */
	/* so aborti, contenti, regexi are now obsolete */
	regex_flags |= REG_ICASE;
	/* Add extended regex search */
	regex_flags |= REG_EXTENDED;
	/* Fill the pattern type */
	if (strcmp(type, "abort") == 0) {
		currItem.type = ABORT;
	} else if (strcmp(type, "abortcontent") == 0) {
		currItem.type = ABORTCONTENT;
	} else if(strcmp(type, "whitelist") == 0) {
		currItem.type = WHITELIST;
	} else if(strcmp(type, "trustuser") == 0) {
		currItem.type = TRUSTUSER;
	} else if(strcmp(type, "trustclient") == 0) {
		currItem.type = TRUSTCLIENT;
	} else if ( (strcmp(type, "squid_ip") != 0) && (strcmp(type, "squid_port") != 0) && (strcmp(type, "maxredir") != 0) && (strcmp(type, "useragent") != 0) && (strcmp(type, "trust_cache") != 0) ) {
		fprintf(stderr, "WARNING: Bad configuration keyword: %s\n", s);
		xfree(type);
		xfree(first);
		return 1;
	}

	/* Fill the pattern flag */
	currItem.flag = regex_flags;

	/* Fill pattern array */
	currItem.pattern = malloc(sizeof(char)*(strlen(first)+1));
	if (currItem.pattern == NULL) {
		fprintf(stderr, "unable to allocate new pattern in add_to_patterns()\n");
		xfree(type);
		xfree(first);
		return 0;
	}
	strncpy(currItem.pattern, first, strlen(first) + 1);
	if ((stored = regcomp(&currItem.regexv, currItem.pattern, currItem.flag)) != 0) {
		ci_debug_printf(0, "ERROR add_pattern: Invalid regex pattern: %s\n", currItem.pattern);
	} else {
		if (growPatternArray(currItem) < 0) {
			fprintf(stderr, "unable to allocate new pattern in add_to_patterns()\n");
			xfree(type);
			xfree(first);
			return 0;
		}
	}
	xfree(type);
	xfree(first);
	return 1;
}