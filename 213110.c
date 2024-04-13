static bool add_argv(RFlagItem *fi, void *user) {
	RLineCompletion *completion = user;
	r_line_completion_push (completion, fi->name);
	return true;
}