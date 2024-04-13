R_API char **r_str_argv(const char *cmdline, int *_argc) {
	int argc = 0;
	int argv_len = 128; // Begin with that, argv will reallocated if necessary
	char *args; // Working buffer for writing unescaped args
	int cmdline_current = 0; // Current character index in _cmdline
	int args_current = 0; // Current character index in  args
	int arg_begin = 0; // Index of the first character of the current argument in args

	if (!cmdline) {
		return NULL;
	}

	char **argv = malloc (argv_len * sizeof (char *));
	if (!argv) {
		return NULL;
	}
	args = malloc (128 + strlen (cmdline) * sizeof (char)); // Unescaped args will be shorter, so strlen (cmdline) will be enough
	if (!args) {
		free (argv);
		return NULL;
	}
	do {
		// States for parsing args
		int escaped = 0;
		int singlequoted = 0;
		int doublequoted = 0;

		// Seek the beginning of next argument (skip whitespaces)
		while (cmdline[cmdline_current] != '\0' && IS_WHITECHAR (cmdline[cmdline_current])) {
			cmdline_current++;
		}

		if (cmdline[cmdline_current] == '\0') {
			break; // No more arguments
		}
		// Read the argument
		while (1) {
			char c = cmdline[cmdline_current];
			int end_of_current_arg = 0;
			if (escaped) {
				switch (c) {
				case '\'':
				case '"':
				case ' ':
				case '\\':
					args[args_current++] = '\\';
					args[args_current++] = c;
					break;
				case '\0':
					args[args_current++] = '\\';
					end_of_current_arg = 1;
					break;
				default:
					args[args_current++] = '\\';
					args[args_current++] = c;
				}
				escaped = 0;
			} else {
				switch (c) {
				case '\'':
					if (doublequoted) {
						args[args_current++] = c;
					} else {
						singlequoted = !singlequoted;
					}
					break;
				case '"':
					if (singlequoted) {
						args[args_current++] = c;
					} else {
						doublequoted = !doublequoted;
					}
					break;
				case '\\':
					escaped = 1;
					break;
				case ' ':
					if (singlequoted || doublequoted) {
						args[args_current++] = c;
					} else {
						end_of_current_arg = 1;
					}
					break;
				case '\0':
					end_of_current_arg = 1;
					break;
				default:
					args[args_current++] = c;
				}
			}
			if (end_of_current_arg) {
				break;
			}
			cmdline_current++;
		}
		args[args_current++] = '\0';
		argv[argc++] = strdup (&args[arg_begin]);
		if (argc >= argv_len) {
			argv_len *= 2;
			char **tmp = realloc (argv, argv_len * sizeof (char *));
			if (!tmp) {
				free (args);
				free (argv);
				return NULL;
			}
			argv = tmp;
		}
		arg_begin = args_current;
	} while (cmdline[cmdline_current++] != '\0');
	argv[argc] = NULL;
	char **tmp = realloc (argv, (argc + 1) * sizeof (char *));
	if (tmp) {
		argv = tmp;
	} else {
		free (argv);
		argv = NULL;
	}
	if (_argc) {
		*_argc = argc;
	}
	free (args);
	return argv;
}