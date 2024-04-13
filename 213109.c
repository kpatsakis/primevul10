static void autocomplete_flags(RCore *core, RLineCompletion *completion, const char* str) {
	r_return_if_fail (str);
	int n = strlen (str);
	r_flag_foreach_prefix (core->flags, str, n, add_argv, completion);
}