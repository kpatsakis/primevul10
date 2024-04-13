R_API char *r_sys_cmd_str(const char *cmd, const char *input, int *len) {
	char *output = NULL;
	if (r_sys_cmd_str_full (cmd, input, &output, len, NULL)) {
		return output;
	}
	free (output);
	return NULL;
}