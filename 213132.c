R_API int r_core_fgets(char *buf, int len) {
	const char *ptr;
	RLine *rli = r_line_singleton ();
	buf[0] = '\0';
	r_line_completion_set (&rli->completion, radare_argc, radare_argv);
 	rli->completion.run = autocomplete;
 	rli->completion.run_user = rli->user;
	ptr = r_line_readline ();
	if (!ptr) {
		return -1;
	}
	strncpy (buf, ptr, len - 1);
	buf[len - 1] = 0;
	return strlen (buf);
}