R_API void r_str_argv_free(char **argv) {
	int argc = 0;
	if (!argv) {
		return;
	}
	while (argv[argc]) {
		free (argv[argc++]);
	}
	free (argv);
}