simple_pattern_compare(char *str, const int type)
{
	int i = 0;  

	/* pass througth all regex pattern */
	for (i = 0; i < pattc; i++) {
		if ( (patterns[i].type == type) && (regexec(&patterns[i].regexv, str, 0, 0, 0) == 0) ) {
			switch(type) {
				/* return 1 if string matches whitelist pattern */
				case WHITELIST:
					if (debug > 0)
						ci_debug_printf(2, "DEBUG simple_pattern_compare: whitelist (%s) matched: %s\n", patterns[i].pattern, str);
					return 1;
				/* return 1 if string matches abort pattern */
				case ABORT:
					if (debug > 0)
						ci_debug_printf(2, "DEBUG simple_pattern_compare: abort (%s) matched: %s\n", patterns[i].pattern, str);
					return 1;
				/* return 1 if string matches trustuser pattern */
				case TRUSTUSER:
					if (debug > 0)
						ci_debug_printf(2, "DEBUG simple_pattern_compare: trustuser (%s) matched: %s\n", patterns[i].pattern, str);
					return 1;
				/* return 1 if string matches abortcontent pattern */
				case ABORTCONTENT:
					if (debug > 0)
						ci_debug_printf(2, "DEBUG simple_pattern_compare: abortcontent (%s) matched: %s\n", patterns[i].pattern, str);
					return 1;
				default:
					ci_debug_printf(0, "ERROR simple_pattern_compare: unknown pattern match type: %s\n", str);
					return -1;
			}
		}
	}

	/* return 0 otherwise */
	return 0;
}