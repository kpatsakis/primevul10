R_API int r_core_prompt_exec(RCore *r) {
	int ret = r_core_cmd (r, r->cmdqueue, true);
	r->rc = r->num->value;
	//int ret = r_core_cmd (r, r->cmdqueue, true);
	if (r->cons && r->cons->use_tts) {
		const char *buf = r_cons_get_buffer();
		r_sys_tts (buf, true);
		r->cons->use_tts = false;
	}
	r_cons_echo (NULL);
	r_cons_flush ();
	if (r->cons && r->cons->line && r->cons->line->zerosep) {
		r_cons_zero ();
	}
	return ret;
}