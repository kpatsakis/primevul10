static int autocomplete(RLineCompletion *completion, RLineBuffer *buf, RLinePromptType prompt_type, void *user) {
	RCore *core = user;
	r_core_autocomplete (core, completion, buf, prompt_type);
	return true;
}