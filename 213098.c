static void autocomplete_functions (RCore *core, RLineCompletion *completion, const char* str) {
	r_return_if_fail (str);
	RListIter *iter;
	RAnalFunction *fcn;
	int n = strlen (str);
	r_list_foreach (core->anal->fcns, iter, fcn) {
		char *name = r_core_anal_fcn_name (core, fcn);
		if (!strncmp (name, str, n)) {
			r_line_completion_push (completion, name);
		}
		free (name);
	}
}