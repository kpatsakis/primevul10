R_API int r_core_prompt(RCore *r, int sync) {
	char line[4096];

	int rnv = r->num->value;
	set_prompt (r);
	int ret = r_cons_fgets (line, sizeof (line), 0, NULL);
	if (ret == -2) {
		return R_CORE_CMD_EXIT; // ^D
	}
	if (ret == -1) {
		return false; // FD READ ERROR
	}
	r->num->value = rnv;
	if (sync) {
		return r_core_prompt_exec (r);
	}
	free (r->cmdqueue);
	r->cmdqueue = strdup (line);
        if (r->scr_gadgets && *line && *line != 'q') {
                r_core_cmd0 (r, "pg");
        }
	r->num->value = r->rc;
	return true;
}