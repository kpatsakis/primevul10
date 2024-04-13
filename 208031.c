client_pattern_compare(char *ip, char *name)
{
	int i = 0;  

	/* pass througth all regex pattern */
	for (i = 0; i < pattc; i++) {
		if (patterns[i].type == TRUSTCLIENT) {
			/* Look at client ip pattern matching */
			/* return 1 if string matches ip TRUSTCLIENT pattern */
			if (regexec(&patterns[i].regexv, ip, 0, 0, 0) == 0) {
				if (debug != 0)
					ci_debug_printf(2, "DEBUG client_pattern_compare: trustclient (%s) matched: %s\n", patterns[i].pattern, ip);
				return 1;
			/* Look at client name pattern matching */
			/* return 2 if string matches fqdn TRUSTCLIENT pattern */
			} else if ((name != NULL) && (regexec(&patterns[i].regexv, name, 0, 0, 0) == 0)) {
				if (debug != 0)
					ci_debug_printf(2, "DEBUG client_pattern_compare: trustclient (%s) matched: %s\n", patterns[i].pattern, name);
				return 2;
			}
		}
	}

	/* return 0 otherwise */
	return 0;
}